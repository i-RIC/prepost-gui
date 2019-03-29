#include "gridcreatingconditionpoisson.h"
#include "gridcreatingconditionpoissonbuildbanklinesdialog.h"
#include "gridcreatingconditionpoissonctrlpointsdialog.h"
#include "gridcreatingconditionpoissongridgeneratedialog.h"
#include "poissonsolver.h"
#include "springsolverthread.h"
#include "springsolver/edge.h"

#include "private/gridcreatingconditionpoisson_addvertexcommand.h"
#include "private/gridcreatingconditionpoisson_coordinateseditor.h"
#include "private/gridcreatingconditionpoisson_definenewpointcommand.h"
#include "private/gridcreatingconditionpoisson_editcoordinatescommand.h"
#include "private/gridcreatingconditionpoisson_finishdefiningcommand.h"
#include "private/gridcreatingconditionpoisson_impl.h"
#include "private/gridcreatingconditionpoisson_movecommand.h"
#include "private/gridcreatingconditionpoisson_movevertexcommand.h"
#include "private/gridcreatingconditionpoisson_removevertexcommand.h"
#include "private/gridcreatingconditionpoisson_updatelabelscommand.h"

#include <geodata/riversurvey/geodatariversurvey.h>
#include <geodata/riversurvey/geodatariverpathpoint.h>
#include <geoio/polylineio.h>
#include <geoio/coordinatesedit.h>
#include <guibase/widget/waitdialog.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatatopdataiteminterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorgridcreatingconditiondataiteminterface.h>
#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <misc/iricundostack.h>
#include <misc/keyboardsupport.h>
#include <misc/mathsupport.h>
#include <misc/xmlsupport.h>
#include <misc/zdepthrange.h>

#include <vtkActor.h>
#include <vtkActor2DCollection.h>
#include <vtkActorCollection.h>
#include <vtkCardinalSpline.h>
#include <vtkParametricSpline.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTextProperty.h>

#include <QAction>
#include <QCoreApplication>
#include <QDataStream>
#include <QFile>
#include <QLocale>
#include <QMenu>
#include <QKeyEvent>
#include <QMessageBox>

namespace {

const int LINEWIDTH_WIDE = 2;
const int LINEWIDTH_NARROW = 1;

const int POINTSIZE = 5;
const int FONTSIZE = 17;

const int SPLINE_FACTOR = 10;

GeoDataRiverSurvey* findRiverSurveyData(GridCreatingCondition* cond)
{
	auto gtItem = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(cond->parent()->parent()->parent());
	auto rtItem = gtItem->geoDataTop();

	for (auto gItem : rtItem->groupDataItems()) {
		auto rItems = gItem->geoDatas();
		for (auto rItem : rItems) {
			auto riverSurvey = dynamic_cast<GeoDataRiverSurvey*>(rItem->geoData());
			if (riverSurvey != nullptr) {
				// this is a river survey data!
				return riverSurvey;
			}
		}
	}
	return nullptr;
}

int getNumPoints(GeoDataRiverSurvey* riverSurvey)
{
	int num = 0;
	GeoDataRiverPathPoint* p = riverSurvey->headPoint();
	while (p->nextPoint() != 0) {
		++ num;
		p = p->nextPoint();
	}

	return num;
}

void makeLineWideWithPoints(PolyLineController* controller)
{
	controller->linesActor()->GetProperty()->SetLineWidth(LINEWIDTH_WIDE);
	controller->pointsActor()->GetProperty()->SetPointSize(POINTSIZE);
}

void makeLineNarrowNoPoints(PolyLineController* controller)
{
	controller->linesActor()->GetProperty()->SetLineWidth(LINEWIDTH_NARROW);
	controller->pointsActor()->GetProperty()->SetPointSize(1);
}

void setupLabelActor(vtkLabel2DActor* actor)
{
	actor->setLabelPosition(vtkLabel2DActor::lpMiddleRight);
	auto prop = actor->labelTextProperty();
	prop->SetFontSize(FONTSIZE);
	prop->BoldOn();
}

const int SUBDIV_NUM = 3;

std::vector<QPointF> buildSplinePoints(vtkPoints* points, int divNum)
{
	auto xSpline = vtkSmartPointer<vtkCardinalSpline>::New();
	auto ySpline = vtkSmartPointer<vtkCardinalSpline>::New();
	auto zSpline = vtkSmartPointer<vtkCardinalSpline>::New();
	auto spline = vtkSmartPointer<vtkParametricSpline>::New();

	spline->SetXSpline(xSpline);
	spline->SetYSpline(ySpline);
	spline->SetZSpline(zSpline);

	spline->SetPoints(points);

	int d = divNum * SUBDIV_NUM;

	std::vector<double> length(d + 1);
	length[0] = 0;
	double u[3], v_prev[3], v[3], Du[9];
	u[0] = 0; u[1] = 0; u[2] = 0;
	spline->Evaluate(u, v_prev, Du);
	for (int i = 1; i <= d; ++i) {
		u[0] = i / static_cast<double>(d);
		spline->Evaluate(u, v, Du);
		double dx = v[0] - v_prev[0];
		double dy = v[1] - v_prev[1];
		length[i] = length[i - 1] + sqrt(dx * dx + dy * dy);

		for (int j = 0; j < 3; ++j) {
			v_prev[j] = v[j];
		}
	}
	double wholeLen = length.at(length.size() - 1);

	int idx = 0;
	std::vector<QPointF> ret;

	for (int i = 0; i <= divNum; ++i) {
		double param = 0;
		if (i != 0) {
			double r = i / static_cast<double>(divNum);
			if (r > 1) {r = 1;}
			double targetLen = wholeLen * r;
			while (length[idx + 1] < targetLen) {
				++ idx;
			}
			double l1 = length[idx];
			double l2 = length[idx + 1];
			double target_idx = idx + 1.0 / (l2 - l1) * (targetLen - l1);
			param = target_idx / d;
		}
		u[0] = param;
		spline->Evaluate(u, v, Du);

		ret.push_back(QPointF(v[0], v[1]));
	}
	return ret;
}

double polyLineLength(std::vector<QPointF>& polyLine)
{
	double len = 0;

	QPointF prev_p = polyLine.at(0);
	for (QPointF p : polyLine) {
		double dx = p.x() - prev_p.x();
		double dy = p.y() - prev_p.y();

		len = len + sqrt(dx * dx + dy * dy);
		prev_p = p;
	}
	return len;
}

void applyOffsetToPolyLine(PolyLineController* polyLine, double x, double y)
{
	auto newLine = CoordinatesEdit::applyOffset(polyLine->polyLine(), - QPointF(x, y));
	polyLine->setPolyLine(newLine);
}

void loadPolyLine(QDataStream* stream, PolyLineController* polyLine, const QPointF& offset)
{
	std::vector<QPointF> line;
	int size;
	*stream >> size;
	for (int i = 0; i < size; ++i) {
		qreal x, y;
		*stream >> x >> y;
		line.push_back(QPointF(x - offset.x(), y - offset.y()));
	}
	polyLine->setPolyLine(line);
}

void savePolyLine(QDataStream* stream, const PolyLineController& polyLine, const QPointF& offset)
{
	auto line = polyLine.polyLine();
	int size = static_cast<int>(line.size());
	*stream << size;
	for (QPointF& p : line) {
		*stream << p.x() + offset.x() << p.y() + offset.y();
	}
}

} // namespace

GridCreatingConditionPoisson::Impl::Impl(GridCreatingConditionPoisson *parent) :
	m_parent {parent},
	m_activePoints {&m_centerLineController},
	m_activeLine {&m_centerLineSplineController},
	m_mouseEventMode {MouseEventMode::BeforeDefining},

	m_iDiv {10},
	m_jDiv {10},
	m_maxIterations {20},

	m_previousLeftBankDistance {10},
	m_previousRightBankDistance {10},

	m_buildBankLinesAction {new QAction(GridCreatingConditionPoisson::tr("Build Left bank and Right bank lines"), parent)},
	m_addVertexAction {new QAction(QIcon(":/libs/guibase/images/iconAddPolygonVertex.png"), GridCreatingConditionPoisson::tr("&Add Vertex"), parent)},
	m_removeVertexAction {new QAction(QIcon(":/libs/guibase/images/iconRemovePolygonVertex.png"), GridCreatingConditionPoisson::tr("&Remove Vertex"), parent)},
	m_importCenterLineAction {new QAction(QIcon(":/libs/guibase/images/iconImport.png"), GridCreatingConditionPoisson::tr("Import C&enter Line..."), parent)},
	m_exportCenterLineAction {new QAction(QIcon(":/libs/guibase/images/iconExport.png"), GridCreatingConditionPoisson::tr("Export Ce&nter Line..."), parent)},
	m_editCenterLineCoordsAction {new QAction(GridCreatingConditionPoisson::tr("Edit Cen&ter Line Coordinates..."), parent)},
	m_importLeftBankLineAction {new QAction(QIcon(":/libs/guibase/images/iconImport.png"), GridCreatingConditionPoisson::tr("Import &Left Bank Line..."), parent)},
	m_exportLeftBankLineAction {new QAction(QIcon(":/libs/guibase/images/iconExport.png"), GridCreatingConditionPoisson::tr("Export Le&ft Bank Line..."), parent)},
	m_editLeftBankLineCoordsAction {new QAction(GridCreatingConditionPoisson::tr("Edit Left Bank Line &Coordinates..."), parent)},
	m_importRightBankLineAction {new QAction(QIcon(":/libs/guibase/images/iconImport.png"), GridCreatingConditionPoisson::tr("&Import Right Bank Line..."), parent)},
	m_exportRightBankLineAction {new QAction(QIcon(":/libs/guibase/images/iconExport.png"), GridCreatingConditionPoisson::tr("Export Ri&ght Bank Line..."), parent)},
	m_editRightBankLineCoordsAction {new QAction(GridCreatingConditionPoisson::tr("Edit Rig&ht Bank Line Coordinates..."), parent)},

	m_addCursorPixmap {":/libs/guibase/images/cursorAdd.png"},
	m_removeCursorPixmap {":/libs/guibase/images/cursorRemove.png"},
	m_addCursor {m_addCursorPixmap, 0, 0},
	m_removeCursor {m_removeCursorPixmap, 0, 0}
{
	m_addVertexAction->setCheckable(true);
	m_removeVertexAction->setCheckable(true);

	connect(m_buildBankLinesAction, SIGNAL(triggered()), parent, SLOT(buildBankLines()));
	connect(m_addVertexAction, SIGNAL(triggered(bool)), parent, SLOT(addVertexMode(bool)));
	connect(m_removeVertexAction, SIGNAL(triggered(bool)), parent, SLOT(removeVertexMode(bool)));
	connect(m_importCenterLineAction, SIGNAL(triggered()), parent, SLOT(importCenterLine()));
	connect(m_exportCenterLineAction, SIGNAL(triggered()), parent, SLOT(exportCenterLine()));
	connect(m_editCenterLineCoordsAction, SIGNAL(triggered()), parent, SLOT(editCenterLineCoordinates()));
	connect(m_importLeftBankLineAction, SIGNAL(triggered()), parent, SLOT(importLeftBankLine()));
	connect(m_exportLeftBankLineAction, SIGNAL(triggered()), parent, SLOT(exportLeftBankLine()));
	connect(m_editLeftBankLineCoordsAction, SIGNAL(triggered()), parent, SLOT(editLeftBankLineCoordinates()));
	connect(m_importRightBankLineAction, SIGNAL(triggered()), parent, SLOT(importRightBankLine()));
	connect(m_exportRightBankLineAction, SIGNAL(triggered()), parent, SLOT(exportRightBankLine()));
	connect(m_editRightBankLineCoordsAction, SIGNAL(triggered()), parent, SLOT(editRightBankLineCoordinates()));
}

GridCreatingConditionPoisson::Impl::~Impl()
{
	delete m_rightClickingMenu;
}

void GridCreatingConditionPoisson::Impl::finishDefiningLine()
{
	if (m_activeLine->polyLine().size() < 2) {
		// not enough points
		return;
	}
	iRICUndoStack::instance().undo();
	m_parent->pushCommand(new FinishDefiningCommand(m_parent));
}

void GridCreatingConditionPoisson::Impl::updateLabelsAndSplines()
{
	auto col = m_parent->actor2DCollection();

	auto center = m_centerLineController.polyLine();
	auto left = m_leftBankLineController.polyLine();
	auto right = m_rightBankLineController.polyLine();

	std::vector<QPointF> empty;
	if (center.size() >= 2) {
		auto centerLineSpline = buildSplinePoints(m_centerLineController.polyData()->GetPoints(), center.size() * SPLINE_FACTOR);
		m_centerLineSplineController.setPolyLine(centerLineSpline);
	} else {
		m_centerLineSplineController.setPolyLine(empty);
	}
	if (left.size() >= 2) {
		auto leftBankLineSpline = buildSplinePoints(m_leftBankLineController.polyData()->GetPoints(), left.size() * SPLINE_FACTOR);
		m_leftBankLineSplineController.setPolyLine(leftBankLineSpline);
	} else {
		m_leftBankLineSplineController.setPolyLine(empty);
	}
	if (right.size() >= 2) {
		auto rightBankLineSpline = buildSplinePoints(m_rightBankLineController.polyData()->GetPoints(), right.size() * SPLINE_FACTOR);
		m_rightBankLineSplineController.setPolyLine(rightBankLineSpline);
	}

	m_upstreamActor.actor()->VisibilityOff();
	m_downstreamActor.actor()->VisibilityOff();
	col->RemoveItem(m_upstreamActor.actor());
	col->RemoveItem(m_downstreamActor.actor());

	m_upstreamLineController.setPolyLine(empty);
	m_downstreamLineController.setPolyLine(empty);

	auto line = m_centerLineController.polyLine();
	if (line.size() < 2) {return;}

	m_upstreamActor.setPosition(line.at(0));
	m_downstreamActor.setPosition(line.at(line.size() - 1));
	col->AddItem(m_upstreamActor.actor());
	col->AddItem(m_downstreamActor.actor());

	if (m_leftBankLineController.polyLine().size() > 0 && m_rightBankLineController.polyLine().size() > 0) {
		std::vector<QPointF> upstream, downstream;

		upstream.push_back(left.at(0));
		upstream.push_back(center.at(0));
		upstream.push_back(right.at(0));

		downstream.push_back(left.at(left.size() - 1));
		downstream.push_back(center.at(center.size() - 1));
		downstream.push_back(right.at(right.size() - 1));

		m_upstreamLineController.setPolyLine(upstream);
		m_downstreamLineController.setPolyLine(downstream);
	}

	m_parent->updateVisibilityWithoutRendering();
}

void GridCreatingConditionPoisson::Impl::updateMouseEventMode(const QPoint& mousePosition)
{
	double x = mousePosition.x();
	double y = mousePosition.y();

	auto v = m_parent->graphicsView();
	v->viewportToWorld(x, y);
	QPointF worldPos(x, y);
	double radius = v->stdRadius(iRIC::nearRadius());

	// switch activeLine
	int tmp_edgeid;
	if (m_leftBankLineSplineController.isEdgeSelectable(worldPos, radius, &tmp_edgeid)){
		m_activePoints = &m_leftBankLineController;
		m_activeLine = &m_leftBankLineSplineController;
	} else if (m_rightBankLineSplineController.isEdgeSelectable(worldPos, radius, &tmp_edgeid)){
		m_activePoints = &m_rightBankLineController;
		m_activeLine = &m_rightBankLineSplineController;
	} else {
		m_activePoints = &m_centerLineController;
		m_activeLine = &m_centerLineSplineController;
	}

	Edge lineEdge;
	switch (m_mouseEventMode) {
	case MouseEventMode::AddVertexNotPossible:
	case MouseEventMode::AddVertexPrepare:
		if (m_activeLine->isEdgeSelectable(worldPos, radius, &m_selectedEdgeId)) {
			lineEdge.setLine(m_activePoints->polyLine());
			m_selectedEdgeId = lineEdge.findNearestLine(worldPos.x(), worldPos.y());
			m_mouseEventMode = MouseEventMode::AddVertexPrepare;
		} else {
			m_mouseEventMode = MouseEventMode::AddVertexNotPossible;
		}
		break;
	case MouseEventMode::RemoveVertexNotPossible:
	case MouseEventMode::RemoveVertexPrepare:
		if (m_activePoints->isVertexSelectable(worldPos, radius, &m_selectedVertexId)) {
			m_mouseEventMode = MouseEventMode::RemoveVertexPrepare;
		} else {
			m_mouseEventMode = MouseEventMode::RemoveVertexNotPossible;
		}
		break;
	case MouseEventMode::Normal:
	case MouseEventMode::MoveVertexPrepare:
	case MouseEventMode::MoveVertex:
	case MouseEventMode::AddVertex:
		if (m_activePoints->isVertexSelectable(worldPos, radius, &m_selectedVertexId)) {
			m_mouseEventMode = MouseEventMode::MoveVertexPrepare;
		} else {
			m_mouseEventMode = MouseEventMode::Normal;
		}
		break;
	case MouseEventMode::BeforeDefining:
	case MouseEventMode::Defining:
	case MouseEventMode::EditCoodinatesDialog:
		// do nothing
		break;
	}
}

void GridCreatingConditionPoisson::Impl::updateMouseCursor(PreProcessorGraphicsViewInterface* v)
{
	switch (m_mouseEventMode) {
	case MouseEventMode::Normal:
	case MouseEventMode::AddVertexNotPossible:
	case MouseEventMode::RemoveVertexNotPossible:
		v->setCursor(Qt::ArrowCursor);
		break;
	case MouseEventMode::BeforeDefining:
	case MouseEventMode::Defining:
		v->setCursor(Qt::CrossCursor);
		break;
	case MouseEventMode::MoveVertexPrepare:
		v->setCursor(Qt::OpenHandCursor);
		break;
	case MouseEventMode::AddVertexPrepare:
		v->setCursor(m_addCursor);
		break;
	case MouseEventMode::RemoveVertexPrepare:
		v->setCursor(m_removeCursor);
		break;
	case MouseEventMode::MoveVertex:
	case MouseEventMode::AddVertex:
		v->setCursor(Qt::ClosedHandCursor);
		break;
	}
}

void GridCreatingConditionPoisson::Impl::updateActionStatus()
{
	switch (m_mouseEventMode) {
	case MouseEventMode::BeforeDefining:
	case MouseEventMode::Defining:
	case MouseEventMode::MoveVertex:
		m_addVertexAction->setDisabled(true);
		m_addVertexAction->setChecked(false);
		m_removeVertexAction->setDisabled(true);
		m_removeVertexAction->setChecked(false);
		break;
	case MouseEventMode::Normal:
	case MouseEventMode::MoveVertexPrepare:
		m_addVertexAction->setEnabled(true);
		m_addVertexAction->setChecked(false);
		m_removeVertexAction->setEnabled(m_activeLine->polyData()->GetPoints()->GetNumberOfPoints() > 2);
		m_removeVertexAction->setChecked(false);
		break;
	case MouseEventMode::AddVertexPrepare:
	case MouseEventMode::AddVertexNotPossible:
	case MouseEventMode::AddVertex:
		m_addVertexAction->setEnabled(true);
		m_addVertexAction->setChecked(true);
		m_removeVertexAction->setDisabled(true);
		m_removeVertexAction->setChecked(false);
		break;
	case MouseEventMode::RemoveVertexPrepare:
	case MouseEventMode::RemoveVertexNotPossible:
		m_addVertexAction->setDisabled(true);
		m_addVertexAction->setChecked(false);
		m_removeVertexAction->setEnabled(true);
		m_removeVertexAction->setChecked(true);
		break;
	}
}

void GridCreatingConditionPoisson::Impl::copyCenterLine(GeoDataRiverSurvey *data, int num)
{
	GeoDataRiverPathPoint* p = data->headPoint();
	std::vector<QPointF> line;

	int numPoints = getNumPoints(data);
	int pointsCreated = 0;
	int idx = 0;

	while (p->nextPoint() != nullptr) {
		p = p->nextPoint();
		double r1 = pointsCreated / static_cast<double>(num - 1);
		double r2 = idx / static_cast<double>(numPoints - 1);
		if (idx == 0 || r1 <= r2) {
			auto pos = p->position();
			QPointF newPoint(pos.x(), pos.y());
			line.push_back(newPoint);
			++ pointsCreated;
		}
		++ idx;
	}
	m_centerLineController.setPolyLine(line);
	if (line.size() < 2) {return;}

	m_mouseEventMode = MouseEventMode::Normal;
	updateLabelsAndSplines();
	updateActionStatus();
}

void GridCreatingConditionPoisson::Impl::importLine(PolyLineController* line)
{
	auto polyLine = CoordinatesEdit::unapplyOffset(
				PolylineIO::importData(m_parent->preProcessorWindow()), m_parent->offset());
	if (polyLine.size() == 0) {return;}

	line->setPolyLine(polyLine);
	m_mouseEventMode = MouseEventMode::Normal;

	updateLabelsAndSplines();
	m_parent->renderGraphicsView();
}

void GridCreatingConditionPoisson::Impl::exportLine(PolyLineController* line, const QString& lineName)
{
	auto l = CoordinatesEdit::applyOffset(line->polyLine(), m_parent->offset());
	if (l.size() == 0) {
		QMessageBox::warning(m_parent->preProcessorWindow(), GridCreatingConditionPoisson::tr("Warning"),
												 GridCreatingConditionPoisson::tr("%1 not defined yet").arg(lineName));
		return;
	}
	PolylineIO::exportData(l, m_parent->preProcessorWindow());
}

Grid* GridCreatingConditionPoisson::Impl::createGrid()
{
	int iMax = m_iDiv + 1;
	int jMax = m_jDiv + 1;
	if (iMax * jMax > MAXGRIDSIZE) {
		QMessageBox::warning(m_parent->preProcessorWindow(), tr("Warning"), tr("The maximum number of grid nodes is %1.").arg(MAXGRIDSIZE));
		return nullptr;
	}

	std::vector<double> xVec(iMax * jMax);
	std::vector<double> yVec(iMax * jMax);

	Structured2DGrid* grid = new Structured2DGrid(nullptr);
	PreProcessorGridTypeDataItemInterface* gt = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(m_parent->m_conditionDataItem->parent()->parent());
	gt->gridType()->buildGridAttributes(grid);

	grid->setDimensions(iMax, jMax);
	vtkPoints* points = vtkPoints::New();
	points->SetDataTypeToDouble();
	points->Allocate(iMax * jMax);

	auto leftBankPoints = buildSplinePoints(m_leftBankLineController.polyData()->GetPoints(), m_iDiv);
	auto rightBankPoints = buildSplinePoints(m_rightBankLineController.polyData()->GetPoints(), m_iDiv);

	double xOffset = 0;
	double yOffset = 0;

	for (int j = 0; j < jMax; ++j) {
		for (int i = 0; i < iMax; ++i) {
			double d = j / static_cast<double>(jMax - 1);
			QPointF p1 = rightBankPoints.at(i);
			QPointF p2 = leftBankPoints.at(i);

			double x = p1.x() * (1 - d) + p2.x() * d;
			double y = p1.y() * (1 - d) + p2.y() * d;

			if (i == 0 && j == 0) {
				xOffset = x;
				yOffset = y;
			}

			xVec[i + j * iMax] = x - xOffset;
			yVec[i + j * iMax] = y - yOffset;
		}
	}

	SpringSolverThread t(m_parent);
	t.setGrid(&xVec, &yVec, iMax, jMax);
	t.setCondition(0.001, m_maxIterations);

	WaitDialog waitDialog(m_parent->preProcessorWindow());
	waitDialog.showProgressBar();
	waitDialog.show();

	connect(&waitDialog, SIGNAL(canceled()), &t, SLOT(cancel()));

	t.start();
	qApp->processEvents();

	while (! t.isFinished()) {
		waitDialog.setProgress(t.progress());
		qApp->processEvents();
	}

//	SpringSolver::solve(&xVec, &yVec, iMax, jMax, 0.001, m_maxIterations);

	for (int i = 0; i < xVec.size(); ++i) {
		points->InsertNextPoint(xVec.at(i) + xOffset, yVec.at(i) + yOffset, 0);
	}

	grid->vtkGrid()->SetPoints(points);
	points->Delete();

	for (GridAttributeContainer* c : grid->gridAttributes()) {
		c->allocate();
	}
	grid->setModified();
	return grid;
}

// public interface

GridCreatingConditionPoisson::GridCreatingConditionPoisson(ProjectDataItem* parent, GridCreatingConditionCreator* creator) :
	GridCreatingCondition(parent, creator),
	impl {new Impl {this}}
{}

GridCreatingConditionPoisson::~GridCreatingConditionPoisson()
{
	auto col2 = actor2DCollection();
	col2->RemoveItem(impl->m_upstreamActor.actor());
	col2->RemoveItem(impl->m_downstreamActor.actor());

	auto col = actorCollection();
	col->RemoveItem(impl->m_centerLineSplineController.linesActor());
	col->RemoveItem(impl->m_centerLineController.pointsActor());
	col->RemoveItem(impl->m_leftBankLineSplineController.linesActor());
	col->RemoveItem(impl->m_leftBankLineController.pointsActor());
	col->RemoveItem(impl->m_rightBankLineSplineController.linesActor());
	col->RemoveItem(impl->m_rightBankLineController.pointsActor());
	col->RemoveItem(impl->m_upstreamLineController.linesActor());
	col->RemoveItem(impl->m_downstreamLineController.linesActor());

	renderer()->RemoveActor2D(impl->m_upstreamActor.actor());
	renderer()->RemoveActor2D(impl->m_downstreamActor.actor());

	renderer()->RemoveActor(impl->m_centerLineSplineController.linesActor());
	renderer()->RemoveActor(impl->m_centerLineController.pointsActor());
	renderer()->RemoveActor(impl->m_leftBankLineSplineController.linesActor());
	renderer()->RemoveActor(impl->m_leftBankLineController.pointsActor());
	renderer()->RemoveActor(impl->m_rightBankLineSplineController.linesActor());
	renderer()->RemoveActor(impl->m_rightBankLineController.pointsActor());
	renderer()->RemoveActor(impl->m_upstreamLineController.linesActor());
	renderer()->RemoveActor(impl->m_downstreamLineController.linesActor());

	delete impl;
}

bool GridCreatingConditionPoisson::init()
{
	return true;
}

void GridCreatingConditionPoisson::showInitialDialog()
{
	auto rs = findRiverSurveyData(this);
	if (rs != nullptr)	{
		int numPoints = getNumPoints(rs);

		int defaultVal = (numPoints - 2) / 2;
		if (defaultVal == 0) {defaultVal = 1;}

		GridCreatingConditionPoissonCtrlPointsDialog dialog(preProcessorWindow());
		dialog.setLocale(iricMainWindow()->locale());
		dialog.setMaximum(numPoints);
		int ret = dialog.exec();
		if (ret == QDialog::Accepted) {
			impl->copyCenterLine(rs, dialog.value());
		}
	} else {
		QMessageBox::information(preProcessorWindow(), tr("Warning"), tr("River Survey data not found. Please define Center Line by yourself."));
		impl->m_mouseEventMode = Impl::MouseEventMode::BeforeDefining;
	}
}

bool GridCreatingConditionPoisson::create(QWidget* /*parent*/)
{
	bool ok = true;
	ok = ok && impl->m_centerLineController.polyLine().size() >= 2;
	ok = ok && impl->m_leftBankLineController.polyLine().size() >= 2;
	ok = ok && impl->m_rightBankLineController.polyLine().size() >= 2;
	if (! ok) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Grid region not defined yet."));
		return false;
	}

	GridCreatingConditionPoissonGridGenerateDialog dialog(preProcessorWindow());

	double iLen = polyLineLength(impl->m_centerLineController.polyLine());
	double jLen = 0.5 * (
				polyLineLength(impl->m_upstreamLineController.polyLine()) +
				polyLineLength(impl->m_downstreamLineController.polyLine()));
	dialog.setILength(iLen);
	dialog.setJLength(jLen);

	dialog.setIDiv(impl->m_iDiv);
	dialog.setJDiv(impl->m_jDiv);
	dialog.setMaxIterations(impl->m_maxIterations);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return false;}

	impl->m_iDiv = dialog.iDiv();
	impl->m_jDiv = dialog.jDiv();
	impl->m_maxIterations = dialog.maxIterations();

	Grid* grid = impl->createGrid();
	emit gridCreated(grid);

	return true;
}

bool GridCreatingConditionPoisson::ready() const
{
	return true;
}

void GridCreatingConditionPoisson::clear()
{
	std::vector<QPointF> empty;

	impl->m_centerLineController.setPolyLine(empty);
	impl->m_rightBankLineController.setPolyLine(empty);
	impl->m_leftBankLineController.setPolyLine(empty);

	impl->m_mouseEventMode = Impl::MouseEventMode::BeforeDefining;
	impl->updateLabelsAndSplines();
	iRICUndoStack::instance().clear();

	renderGraphicsView();
}

void GridCreatingConditionPoisson::setupActors()
{
	impl->m_upstreamActor.setLabel("Upstream");
	setupLabelActor(&(impl->m_upstreamActor));
	impl->m_downstreamActor.setLabel("Downstream");
	setupLabelActor(&(impl->m_downstreamActor));

	impl->m_upstreamActor.actor()->VisibilityOff();
	impl->m_downstreamActor.actor()->VisibilityOff();

	renderer()->AddActor2D(impl->m_upstreamActor.actor());
	renderer()->AddActor2D(impl->m_downstreamActor.actor());
	renderer()->AddActor(impl->m_centerLineSplineController.linesActor());
	renderer()->AddActor(impl->m_centerLineController.pointsActor());
	renderer()->AddActor(impl->m_leftBankLineSplineController.linesActor());
	renderer()->AddActor(impl->m_leftBankLineController.pointsActor());
	renderer()->AddActor(impl->m_rightBankLineSplineController.linesActor());
	renderer()->AddActor(impl->m_rightBankLineController.pointsActor());
	renderer()->AddActor(impl->m_upstreamLineController.linesActor());
	renderer()->AddActor(impl->m_downstreamLineController.linesActor());

	auto col = actorCollection();
	col->AddItem(impl->m_centerLineSplineController.linesActor());
	col->AddItem(impl->m_centerLineController.pointsActor());
	col->AddItem(impl->m_leftBankLineSplineController.linesActor());
	col->AddItem(impl->m_leftBankLineController.pointsActor());
	col->AddItem(impl->m_rightBankLineSplineController.linesActor());
	col->AddItem(impl->m_rightBankLineController.pointsActor());
	col->AddItem(impl->m_upstreamLineController.linesActor());
	col->AddItem(impl->m_downstreamLineController.linesActor());
}

void GridCreatingConditionPoisson::setupMenu()
{
	m_menu->addAction(impl->m_buildBankLinesAction);
	m_menu->addSeparator();
	m_menu->addAction(impl->m_addVertexAction);
	m_menu->addAction(impl->m_removeVertexAction);
	m_menu->addSeparator();
	m_menu->addAction(impl->m_editCenterLineCoordsAction);
	m_menu->addAction(impl->m_editLeftBankLineCoordsAction);
	m_menu->addAction(impl->m_editRightBankLineCoordsAction);
	m_menu->addSeparator();
	m_menu->addAction(impl->m_importCenterLineAction);
	m_menu->addAction(impl->m_importLeftBankLineAction);
	m_menu->addAction(impl->m_importRightBankLineAction);
	m_menu->addSeparator();
	m_menu->addAction(impl->m_exportCenterLineAction);
	m_menu->addAction(impl->m_exportLeftBankLineAction);
	m_menu->addAction(impl->m_exportRightBankLineAction);
	m_menu->addSeparator();
	m_menu->addAction(m_conditionDataItem->importAction());
	m_menu->addAction(m_conditionDataItem->exportAction());

	impl->m_rightClickingMenu = new QMenu();
	auto m = impl->m_rightClickingMenu;
	m->addAction(m_conditionDataItem->createAction());
	m->addSeparator();
	m->addAction(impl->m_buildBankLinesAction);
	m->addSeparator();
	m->addAction(impl->m_addVertexAction);
	m->addAction(impl->m_removeVertexAction);
	m->addSeparator();
	m->addAction(impl->m_editCenterLineCoordsAction);
	m->addAction(impl->m_editLeftBankLineCoordsAction);
	m->addAction(impl->m_editRightBankLineCoordsAction);
	m->addSeparator();
	m->addAction(m_conditionDataItem->clearAction());
	m->addSeparator();
	m->addAction(impl->m_importCenterLineAction);
	m->addAction(impl->m_importLeftBankLineAction);
	m->addAction(impl->m_importRightBankLineAction);
	m->addSeparator();
	m->addAction(impl->m_exportCenterLineAction);
	m->addAction(impl->m_exportLeftBankLineAction);
	m->addAction(impl->m_exportRightBankLineAction);
	m_menu->addSeparator();
	m_menu->addAction(m_conditionDataItem->importAction());
	m_menu->addAction(m_conditionDataItem->exportAction());
}

void GridCreatingConditionPoisson::informSelection(PreProcessorGraphicsViewInterface* v)
{
	makeLineWideWithPoints(&(impl->m_centerLineController));
	makeLineWideWithPoints(&(impl->m_centerLineSplineController));
	makeLineWideWithPoints(&(impl->m_leftBankLineController));
	makeLineWideWithPoints(&(impl->m_leftBankLineSplineController));
	makeLineWideWithPoints(&(impl->m_rightBankLineController));
	makeLineWideWithPoints(&(impl->m_rightBankLineSplineController));
	makeLineWideWithPoints(&(impl->m_upstreamLineController));
	makeLineWideWithPoints(&(impl->m_downstreamLineController));
	impl->updateMouseCursor(v);
}

void GridCreatingConditionPoisson::informDeselection(PreProcessorGraphicsViewInterface* v)
{
	makeLineNarrowNoPoints(&(impl->m_centerLineController));
	makeLineNarrowNoPoints(&(impl->m_centerLineSplineController));
	makeLineNarrowNoPoints(&(impl->m_leftBankLineController));
	makeLineNarrowNoPoints(&(impl->m_leftBankLineSplineController));
	makeLineNarrowNoPoints(&(impl->m_rightBankLineController));
	makeLineNarrowNoPoints(&(impl->m_rightBankLineSplineController));
	makeLineNarrowNoPoints(&(impl->m_upstreamLineController));
	makeLineNarrowNoPoints(&(impl->m_downstreamLineController));
	v->unsetCursor();
}

void GridCreatingConditionPoisson::viewOperationEnded(PreProcessorGraphicsViewInterface* v)
{
	impl->updateMouseCursor(v);
}

void GridCreatingConditionPoisson::keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (! iRIC::isEnterKey(event->key())) {return;}
	if (impl->m_mouseEventMode != Impl::MouseEventMode::Defining) {return;}

	impl->finishDefiningLine();
}

void GridCreatingConditionPoisson::keyReleaseEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/)
{}

void GridCreatingConditionPoisson::mouseDoubleClickEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (impl->m_mouseEventMode == Impl::MouseEventMode::Defining) {
		impl->finishDefiningLine();
	}
}

void GridCreatingConditionPoisson::mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	switch (impl->m_mouseEventMode) {
	case Impl::MouseEventMode::Normal:
	case Impl::MouseEventMode::MoveVertexPrepare:
	case Impl::MouseEventMode::AddVertexPrepare:
	case Impl::MouseEventMode::AddVertexNotPossible:
	case Impl::MouseEventMode::RemoveVertexPrepare:
	case Impl::MouseEventMode::RemoveVertexNotPossible:
		impl->updateMouseEventMode(event->pos());
		impl->updateMouseCursor(v);
		break;
	case Impl::MouseEventMode::Defining:
		pushUpdateLabelsCommand(new DefineNewPointCommand(false, event->pos(), this));
		break;
	case Impl::MouseEventMode::MoveVertex:
		pushUpdateLabelsCommand(new MoveVertexCommand(false, impl->m_previousPos, event->pos(), impl->m_selectedVertexId, this));
		break;
	case Impl::MouseEventMode::AddVertex:
		 pushUpdateLabelsCommand(new AddVertexCommand(false, impl->m_selectedEdgeId, event->pos(), this));
		break;
	default:
		break;
	}
	impl->m_previousPos = event->pos();
}

void GridCreatingConditionPoisson::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {
		switch (impl->m_mouseEventMode) {
		case Impl::MouseEventMode::Normal:
			// @todo try switch selection

			break;
		case Impl::MouseEventMode::BeforeDefining:
			impl->m_mouseEventMode = Impl::MouseEventMode::Defining;
			pushUpdateLabelsCommand(new DefineNewPointCommand(true, event->pos(), this));
		case Impl::MouseEventMode::Defining:
			pushUpdateLabelsCommand(new DefineNewPointCommand(true, event->pos(), this));
			break;
		case Impl::MouseEventMode::MoveVertexPrepare:
			impl->m_mouseEventMode = Impl::MouseEventMode::MoveVertex;
			pushUpdateLabelsCommand(new MoveVertexCommand(true, event->pos(), event->pos(), impl->m_selectedVertexId, this));
			break;
		case Impl::MouseEventMode::AddVertexPrepare:
			impl->m_mouseEventMode = Impl::MouseEventMode::AddVertex;
			pushUpdateLabelsCommand(new AddVertexCommand(true, impl->m_selectedEdgeId, event->pos(), this));
			break;
		case Impl::MouseEventMode::RemoveVertexPrepare:
			impl->m_mouseEventMode = Impl::MouseEventMode::Normal;
			pushUpdateLabelsCommand(new RemoveVertexCommand(impl->m_selectedVertexId, this));
			break;
		default:
			break;
		}
		impl->updateMouseCursor(v);
		impl->updateActionStatus();
	}

	impl->m_previousPos = event->pos();
	impl->m_pressPos = event->pos();
}

void GridCreatingConditionPoisson::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {
		impl->updateMouseEventMode(event->pos());
		impl->updateMouseCursor(v);
		impl->updateActionStatus();
	} else if (event->button() == Qt::RightButton) {
		if (impl->m_mouseEventMode == Impl::MouseEventMode::EditCoodinatesDialog) {return;}

		if (iRIC::isNear(impl->m_pressPos, event->pos())) {
			impl->m_rightClickingMenu->move(event->globalPos());
			impl->m_rightClickingMenu->show();
		}
	}
}

void GridCreatingConditionPoisson::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

void GridCreatingConditionPoisson::assignActorZValues(const ZDepthRange& range)
{
	impl->m_centerLineController.pointsActor()->SetPosition(0, 0, range.max());
	impl->m_centerLineSplineController.linesActor()->SetPosition(0, 0, range.max());

	impl->m_leftBankLineController.pointsActor()->SetPosition(0, 0, range.max());
	impl->m_leftBankLineSplineController.linesActor()->SetPosition(0, 0, range.max());

	impl->m_rightBankLineController.pointsActor()->SetPosition(0, 0, range.max());
	impl->m_rightBankLineSplineController.linesActor()->SetPosition(0, 0, range.max());

	impl->m_upstreamLineController.linesActor()->SetPosition(0, 0, range.max());
	impl->m_downstreamLineController.linesActor()->SetPosition(0, 0, range.max());
}

void GridCreatingConditionPoisson::restoreMouseEventMode()
{
	impl->m_mouseEventMode = Impl::MouseEventMode::Normal;
}

void GridCreatingConditionPoisson::addVertexMode(bool on)
{
	if (on) {
		impl->m_mouseEventMode = Impl::MouseEventMode::AddVertexNotPossible;
	} else {
		impl->m_mouseEventMode = Impl::MouseEventMode::Normal;
	}
	impl->updateActionStatus();
}

void GridCreatingConditionPoisson::removeVertexMode(bool on)
{
	if (on) {
		impl->m_mouseEventMode = Impl::MouseEventMode::RemoveVertexNotPossible;
	} else {
		impl->m_mouseEventMode = Impl::MouseEventMode::Normal;
	}
	impl->updateActionStatus();
}

void GridCreatingConditionPoisson::buildBankLines()
{
	auto centerLine = impl->m_centerLineController.polyLine();
	if (centerLine.size() < 2) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Center Line is not defined yet."));
		return;
	}

	GridCreatingConditionPoissonBuildBankLinesDialog dialog(preProcessorWindow());
	dialog.setLeftBankDistance(impl->m_previousLeftBankDistance);
	dialog.setRightBankDistance(impl->m_previousRightBankDistance);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	double leftDistance = dialog.leftBankDistance();
	double rightDistance = dialog.rightBankDistance();

	std::vector<QPointF> leftBankLine, rightBankLine;

	for (int i = 0; i < centerLine.size(); ++i) {
		QPointF v;
		if (i == 0) {
			QPointF p1 = centerLine.at(0);
			QPointF p2 = centerLine.at(1);
			v = iRIC::normalize(p2 - p1);
			iRIC::rotateVector270(v);
		} else if (i == centerLine.size() - 1) {
			QPointF p1 = centerLine.at(centerLine.size() - 2);
			QPointF p2 = centerLine.at(centerLine.size() - 1);
			v = iRIC::normalize(p2 - p1);
			iRIC::rotateVector270(v);
		} else {
			QPointF p1 = centerLine.at(i - 1);
			QPointF p2 = centerLine.at(i);
			QPointF p3 = centerLine.at(i + 1);
			QPointF v1 = iRIC::normalize(p2 - p1);
			iRIC::rotateVector270(v1);
			QPointF v2 = iRIC::normalize(p3 - p2);
			iRIC::rotateVector270(v2);
			v = iRIC::normalize(v1 + v2);
		}

		QPointF lb = centerLine.at(i);
		lb.setX(lb.x() - leftDistance * v.x());
		lb.setY(lb.y() - leftDistance * v.y());
		leftBankLine.push_back(lb);

		QPointF rb = centerLine.at(i);
		rb.setX(rb.x() + rightDistance * v.x());
		rb.setY(rb.y() + rightDistance * v.y());
		rightBankLine.push_back(rb);
	}

	impl->m_leftBankLineController.setPolyLine(leftBankLine);
	impl->m_rightBankLineController.setPolyLine(rightBankLine);

	impl->m_previousLeftBankDistance = leftDistance;
	impl->m_previousRightBankDistance = rightDistance;

	impl->updateLabelsAndSplines();

	renderGraphicsView();
	iRICUndoStack::instance().clear();
}

void GridCreatingConditionPoisson::importCenterLine()
{
	impl->importLine(&(impl->m_centerLineController));
}

void GridCreatingConditionPoisson::exportCenterLine()
{
	impl->exportLine(&(impl->m_centerLineController), tr("Center line"));
}

void GridCreatingConditionPoisson::editCenterLineCoordinates()
{
	CoordinatesEditor::edit(&(impl->m_centerLineController), tr("Center line"), this);
}

void GridCreatingConditionPoisson::importLeftBankLine()
{
	impl->importLine(&(impl->m_leftBankLineController));
}

void GridCreatingConditionPoisson::exportLeftBankLine()
{
	impl->exportLine(&(impl->m_leftBankLineController), tr("Left bank line"));
}

void GridCreatingConditionPoisson::editLeftBankLineCoordinates()
{
	CoordinatesEditor::edit(&(impl->m_leftBankLineController), tr("Left bank line"), this);
}

void GridCreatingConditionPoisson::importRightBankLine()
{
	impl->importLine(&(impl->m_rightBankLineController));
}

void GridCreatingConditionPoisson::exportRightBankLine()
{
	impl->exportLine(&(impl->m_rightBankLineController), tr("Right bank line"));
}

void GridCreatingConditionPoisson::editRightBankLineCoordinates()
{
	CoordinatesEditor::edit(&(impl->m_rightBankLineController), tr("Right bank line"), this);
}

void GridCreatingConditionPoisson::doLoadFromProjectMainFile(const QDomNode& node)
{
	impl->m_iDiv = iRIC::getIntAttribute(node, "iDiv", 10);
	impl->m_jDiv = iRIC::getIntAttribute(node, "jDiv", 10);
	impl->m_maxIterations = iRIC::getIntAttribute(node, "maxIter", 20);
	impl->m_previousLeftBankDistance = iRIC::getDoubleAttribute(node, "leftDistance", 10);
	impl->m_previousRightBankDistance = iRIC::getDoubleAttribute(node, "rightDistance", 10);
}

void GridCreatingConditionPoisson::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	iRIC::setIntAttribute(writer, "iDiv", impl->m_iDiv);
	iRIC::setIntAttribute(writer, "jDiv", impl->m_jDiv);
	iRIC::setIntAttribute(writer, "maxIter", impl->m_maxIterations);
	iRIC::setDoubleAttribute(writer, "leftDistance", impl->m_previousLeftBankDistance);
	iRIC::setDoubleAttribute(writer, "rightDistance", impl->m_previousRightBankDistance);
}

void GridCreatingConditionPoisson::loadExternalData(const QString& filename)
{
	QFile f(filename);
	f.open(QIODevice::ReadOnly);
	QDataStream s(&f);

	loadPolyLine(&s, &(impl->m_centerLineController), offset());
	loadPolyLine(&s, &(impl->m_leftBankLineController), offset());
	loadPolyLine(&s, &(impl->m_rightBankLineController), offset());

	f.close();
	if (impl->m_centerLineController.polyLine().size() > 0) {
		impl->m_mouseEventMode = Impl::MouseEventMode::Normal;
	} else {
		impl->m_mouseEventMode = Impl::MouseEventMode::BeforeDefining;
	}
	impl->updateLabelsAndSplines();
	impl->updateActionStatus();
}

void GridCreatingConditionPoisson::saveExternalData(const QString& filename)
{
	QFile f(filename);
	f.open(QIODevice::WriteOnly);
	QDataStream s(&f);

	savePolyLine(&s, impl->m_centerLineController, offset());
	savePolyLine(&s, impl->m_leftBankLineController, offset());
	savePolyLine(&s, impl->m_rightBankLineController, offset());

	f.close();
}

void GridCreatingConditionPoisson::doApplyOffset(double x, double y)
{
	applyOffsetToPolyLine(&(impl->m_centerLineController), x, y);
	applyOffsetToPolyLine(&(impl->m_leftBankLineController), x, y);
	applyOffsetToPolyLine(&(impl->m_rightBankLineController), x, y);

	impl->updateLabelsAndSplines();
}

void GridCreatingConditionPoisson::pushUpdateLabelsCommand(QUndoCommand* com, bool renderRedoOnly)
{
	if (renderRedoOnly) {
		pushRenderRedoOnlyCommand(new UpdateLabelsCommand(com, this));
	} else {
		pushRenderCommand(new UpdateLabelsCommand(com, this));
	}
}
