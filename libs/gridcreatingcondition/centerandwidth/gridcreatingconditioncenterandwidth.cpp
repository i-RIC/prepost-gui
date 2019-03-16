#include "gridcreatingconditioncenterandwidth.h"
#include "gridcreatingconditioncenterandwidthdialog.h"
#include "private/gridcreatingconditioncenterandwidth_addvertexcommand.h"
#include "private/gridcreatingconditioncenterandwidth_coordinateseditor.h"
#include "private/gridcreatingconditioncenterandwidth_definenewpointcommand.h"
#include "private/gridcreatingconditioncenterandwidth_finishdefiningcommand.h"
#include "private/gridcreatingconditioncenterandwidth_movecommand.h"
#include "private/gridcreatingconditioncenterandwidth_movevertexcommand.h"
#include "private/gridcreatingconditioncenterandwidth_removevertexcommand.h"
#include "private/gridcreatingconditioncenterandwidth_updateshapecommand.h"

#include <geoio/polylineio.h>
#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorgridcreatingconditiondataiteminterface.h>
#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/container/gridattributerealnodecontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/informationdialog.h>
#include <misc/iricundostack.h>
#include <misc/keyboardsupport.h>
#include <misc/mathsupport.h>
#include <misc/xmlsupport.h>
#include <misc/zdepthrange.h>

#include <QDomNode>
#include <QFile>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QToolBar>
#include <QXmlStreamWriter>

#include <vtkActor2DCollection.h>
#include <vtkActorCollection.h>
#include <vtkCardinalSpline.h>
#include <vtkCollectionIterator.h>
#include <vtkLine.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricSpline.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkTextProperty.h>
#include <vtkVertex.h>

#include <vector>

#define LABEL "label"

namespace {

const int FONTSIZE = 17;

void setupLabelActor(vtkLabel2DActor* actor)
{
	actor->setLabelPosition(vtkLabel2DActor::lpMiddleRight);
	auto prop = actor->labelTextProperty();
	prop->SetFontSize(FONTSIZE);
	prop->BoldOn();
}

} // namespace

GridCreatingConditionCenterAndWidth::GridCreatingConditionCenterAndWidth(ProjectDataItem* parent, GridCreatingConditionCreator* creator) :
	GridCreatingCondition(parent, creator),
	m_previewGrid {nullptr},
	m_rightClickingMenu {nullptr}
{
	m_upstreamActor.setLabel("Upstream");
	setupLabelActor(&m_upstreamActor);

	m_downstreamActor.setLabel("Downstream");
	setupLabelActor(&m_downstreamActor);

	m_addVertexAction = new QAction(QIcon(":/libs/guibase/images/iconAddPolygonVertex.png"), GridCreatingConditionCenterAndWidth::tr("&Add Vertex"), this);
	m_addVertexAction->setCheckable(true);
	connect(m_addVertexAction, SIGNAL(triggered(bool)), this, SLOT(addVertexMode(bool)));
	m_removeVertexAction = new QAction(QIcon(":/libs/guibase/images/iconRemovePolygonVertex.png"), GridCreatingConditionCenterAndWidth::tr("&Remove Vertex"), this);
	m_removeVertexAction->setCheckable(true);
	connect(m_removeVertexAction, SIGNAL(triggered(bool)), this, SLOT(removeVertexMode(bool)));

	m_coordEditAction = new QAction(GridCreatingConditionCenterAndWidth::tr("Edit C&oordinates..."), this);
	connect(m_coordEditAction, SIGNAL(triggered()), this, SLOT(editCoordinates()));

	m_reverseCenterLineAction = new QAction(GridCreatingConditionCenterAndWidth::tr("R&everse Center Line Direction"), this);
	connect(m_reverseCenterLineAction, SIGNAL(triggered()), this, SLOT(reverseCenterLineDirection()));

	m_importCenterLineAction = new QAction(QIcon(":/libs/guibase/images/iconImport.png"), GridCreatingConditionCenterAndWidth::tr("&Import Center Line..."), this);
	connect(m_importCenterLineAction, SIGNAL(triggered()), this, SLOT(importCenterLine()));

	m_exportCenterLineAction = new QAction(QIcon(":/libs/guibase/images/iconExport.png"), GridCreatingConditionCenterAndWidth::tr("&Export Center Line..."), this);
	connect(m_exportCenterLineAction, SIGNAL(triggered()), this, SLOT(exportCenterLine()));

	m_xSpline = vtkSmartPointer<vtkCardinalSpline>::New();
	m_ySpline = vtkSmartPointer<vtkCardinalSpline>::New();
	m_zSpline = vtkSmartPointer<vtkCardinalSpline>::New();
	m_spline = vtkSmartPointer<vtkParametricSpline>::New();
	m_spline->SetXSpline(m_xSpline);
	m_spline->SetYSpline(m_ySpline);
	m_spline->SetZSpline(m_zSpline);
	m_splinePoints = vtkPoints::New();
	m_splinePoints->SetDataTypeToDouble();

	// Set cursors for mouse view change events.
	m_addPixmap = QPixmap(":/libs/guibase/images/cursorAdd.png");
	m_removePixmap = QPixmap(":/libs/guibase/images/cursorRemove.png");
	m_addCursor = QCursor(m_addPixmap, 0, 0);
	m_removeCursor = QCursor(m_removePixmap, 0, 0);

	m_mouseEventMode = meBeforeDefining;

	m_selectedVertexId = 0;
	m_selectedEdgeId = 0;

	m_iMax = 11;
	m_jMax = 11;
	m_width = 10.0;

	m_isAccepted = false;
	m_isGridCreated = false;

	updateActionStatus();
	graphicsView()->ResetCameraClippingRange();
}

GridCreatingConditionCenterAndWidth::~GridCreatingConditionCenterAndWidth()
{
	renderer()->RemoveActor(m_previewActor);
	renderer()->RemoveActor(m_polyLineController.pointsActor());
	renderer()->RemoveActor(m_polyLineController.linesActor());
	renderer()->RemoveActor2D(m_upstreamActor.actor());
	renderer()->RemoveActor2D(m_downstreamActor.actor());

	delete m_rightClickingMenu;
}

std::vector<QPointF> GridCreatingConditionCenterAndWidth::polyLine()
{
	return m_polyLineController.polyLine();
}

void GridCreatingConditionCenterAndWidth::setPolyLine(const std::vector<QPointF>& polyline)
{
	m_polyLineController.setPolyLine(polyline);
}

void GridCreatingConditionCenterAndWidth::updateShapeData()
{
	auto col = actor2DCollection();

	m_upstreamActor.actor()->VisibilityOff();
	m_downstreamActor.actor()->VisibilityOff();
	col->RemoveItem(m_upstreamActor.actor());
	col->RemoveItem(m_downstreamActor.actor());

	auto polyLine = m_polyLineController.polyLine();
	if (polyLine.size() < 2) {return;}

	m_upstreamActor.setPosition(polyLine.at(0));
	m_downstreamActor.setPosition(polyLine.at(polyLine.size() - 1));
	col->AddItem(m_upstreamActor.actor());
	col->AddItem(m_downstreamActor.actor());

	updateVisibilityWithoutRendering();
}

bool GridCreatingConditionCenterAndWidth::create(QWidget* parent)
{
	if (m_mouseEventMode == meBeforeDefining || m_mouseEventMode == meDefining) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Grid creating condition definition not finished yet."));
		return false;
	}
	if (polyLine().size() < 2) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Grid center line is not defined yet. Please click in the main region to define the grid center line."));
		return false;
	}

	createSpline(m_polyLineController.polyData()->GetPoints(), m_iMax);
	showDialog(parent);
	if (! m_isAccepted) {return false;}

	Grid* grid = createGrid();
	if (grid == 0) {return false;}
	m_isGridCreated = true;

	emit gridCreated(grid);
	return true;
}

Grid* GridCreatingConditionCenterAndWidth::createGrid()
{
	if (m_iMax * m_jMax > MAXGRIDSIZE) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("The maximum number of grid nodes is %1.").arg(MAXGRIDSIZE));
		return nullptr;
	}
	createSpline(m_polyLineController.polyData()->GetPoints(), m_iMax - 1);

	Structured2DGrid* grid = new Structured2DGrid(0);
	PreProcessorGridTypeDataItemInterface* gt = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(m_conditionDataItem->parent()->parent());
	gt->gridType()->buildGridAttributes(grid);

	grid->setDimensions(m_iMax, m_jMax);
	vtkPoints* points = vtkPoints::New();
	points->SetDataTypeToDouble();
	points->Allocate(m_iMax * m_jMax);

	for (int j = 0; j < m_jMax; j++) {
		for (int i = 0; i < m_iMax; i++) {
			double p1[3], p2[3];
			if (i == 0) {
				m_splinePoints->GetPoint(i, p1);
				m_splinePoints->GetPoint(i + 1, p2);
			} else if (i == m_iMax - 1) {
				m_splinePoints->GetPoint(i, p1);
				m_splinePoints->GetPoint(i - 1, p2);
			} else {
				m_splinePoints->GetPoint(i - 1, p1);
				m_splinePoints->GetPoint(i + 1, p2);
			}
			double dx = p2[0] - p1[0];
			double dy = p2[1] - p1[1];
			double s = sqrt(dx * dx + dy * dy);
			dx = (m_width / (m_jMax - 1)) * dx / s;
			dy = (m_width / (m_jMax - 1)) * dy / s;
			int center = (m_jMax - 1) / 2;
			if (i == 0) {
				points->InsertPoint(m_iMax * j + i, p1[0] + ((- 1) * center + j) * (- dy), p1[1] + ((- 1) * center + j) * dx, 0.0);
			} else if (i == m_iMax - 1) {
				points->InsertPoint(m_iMax * j + i, p1[0] + (center - j) * (- dy), p1[1] + (center - j) * dx, 0.0);
			} else {
				m_splinePoints->GetPoint(i, p1);
				points->InsertPoint(m_iMax * j + i, p1[0] + ((- 1) * center + j) * (- dy), p1[1] + ((- 1) * center + j) * dx, 0.0);
			}
		}
	}
	grid->vtkGrid()->SetPoints(points);
	points->Delete();

	// allocate memory for all grid related conditions.
	for (GridAttributeContainer* c : grid->gridAttributes()) {
		c->allocate();
	}
	grid->setModified();
	return grid;
}

void GridCreatingConditionCenterAndWidth::showDialog(QWidget* parent)
{
	GridCreatingConditionCenterAndWidthDialog* dialog = new GridCreatingConditionCenterAndWidthDialog(parent);
	connect(dialog, SIGNAL(applied(QDialog*)), this, SLOT(handleDialogApplied(QDialog*)));
	dialog->setLength(m_length);
	dialog->setWidth(m_width);
	dialog->setIMax(m_iMax);
	dialog->setJMax(m_jMax);
	m_oldIMax = m_iMax;
	m_oldJMax = m_jMax;
	m_oldWidth = m_width;

	int result = dialog->exec();
	if (result == QDialog::Accepted) {
		handleDialogAccepted(dialog);
		m_isAccepted = true;
	} else {
		handleDialogRejected(dialog);
		m_isAccepted = false;
	}
	delete dialog;
}

void GridCreatingConditionCenterAndWidth::handleDialogApplied(QDialog* d)
{
	GridCreatingConditionCenterAndWidthDialog* dialog = dynamic_cast<GridCreatingConditionCenterAndWidthDialog*>(d);
	setIMax(dialog->iMax());
	setJMax(dialog->jMax());
	setWidth(dialog->width());

	createSpline(m_polyLineController.polyData()->GetPoints(), m_iMax - 1);

	Grid* g = createGrid();
	if (g == 0) {return;}
	if (m_previewGrid != nullptr) {delete m_previewGrid;}
	m_previewGrid = g;

	m_previewMapper->SetInputData(m_previewGrid->vtkGrid());
	m_previewActor->VisibilityOn();
	renderGraphicsView();
}

void GridCreatingConditionCenterAndWidth::handleDialogAccepted(QDialog* d)
{
	GridCreatingConditionCenterAndWidthDialog* dialog = dynamic_cast<GridCreatingConditionCenterAndWidthDialog*>(d);
	setIMax(dialog->iMax());
	setJMax(dialog->jMax());
	setWidth(dialog->width());
	createSpline(m_polyLineController.polyData()->GetPoints(), m_iMax - 1);

	if (m_previewGrid != nullptr) {
		delete m_previewGrid;
		m_previewGrid = nullptr;
	}
	m_previewActor->VisibilityOff();
}

void GridCreatingConditionCenterAndWidth::handleDialogRejected(QDialog* /*d*/)
{
	setIMax(m_oldIMax);
	setJMax(m_oldJMax);
	setWidth(m_oldWidth);
	m_splinePoints->Initialize();

	if (m_previewGrid != nullptr) {
		delete m_previewGrid;
		m_previewGrid = nullptr;
	}
	m_previewActor->VisibilityOff();

	renderGraphicsView();
}

bool GridCreatingConditionCenterAndWidth::ready() const
{
	return true;
}

void GridCreatingConditionCenterAndWidth::setupActors()
{
	m_polyLineController.linesActor()->GetProperty()->SetLineWidth(2);

	renderer()->AddActor(m_polyLineController.linesActor());
	renderer()->AddActor(m_polyLineController.pointsActor());
	renderer()->AddActor2D(m_upstreamActor.actor());
	renderer()->AddActor2D(m_downstreamActor.actor());

	m_previewActor = vtkSmartPointer<vtkActor>::New();
	m_previewActor->GetProperty()->SetLighting(false);
	m_previewActor->GetProperty()->SetColor(0, 0, 0);
	m_previewActor->GetProperty()->SetRepresentationToWireframe();

	m_previewMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	m_previewActor->SetMapper(m_previewMapper);
	renderer()->AddActor(m_previewActor);
	m_previewActor->VisibilityOff();

//	m_labelMapper = vtkSmartPointer<vtkLabeledDataMapper>::New();
//	m_labelMapper->SetLabelModeToLabelFieldData();
//	m_labelMapper->SetFieldDataName(LABEL);
//	vtkTextProperty* prop = m_labelMapper->GetLabelTextProperty();
//	prop->SetColor(0, 0, 0);
//	prop->SetFontSize(FONTSIZE);
//	prop->SetFontFamilyToArial();
//	prop->BoldOn();
//	prop->ItalicOff();
//	prop->ShadowOff();
//	prop->SetJustificationToLeft();
//	prop->SetVerticalJustificationToCentered();
//	m_labelActor->SetMapper(m_labelMapper);
//	m_labelMapper->SetInputData(m_vertexGrid);

	actorCollection()->AddItem(m_polyLineController.linesActor());
	actorCollection()->AddItem(m_polyLineController.pointsActor());
	actor2DCollection()->AddItem(m_upstreamActor.actor());
	actor2DCollection()->AddItem(m_downstreamActor.actor());

	updateShapeData();
}

void GridCreatingConditionCenterAndWidth::setupMenu()
{
	m_menu->addAction(m_addVertexAction);
	m_menu->addAction(m_removeVertexAction);
	m_menu->addAction(m_coordEditAction);
	m_menu->addAction(m_reverseCenterLineAction);
	m_menu->addSeparator();
	m_menu->addAction(m_importCenterLineAction);
	m_menu->addAction(m_exportCenterLineAction);

	m_rightClickingMenu = new QMenu();
	m_rightClickingMenu->addAction(m_conditionDataItem->createAction());
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(m_addVertexAction);
	m_rightClickingMenu->addAction(m_removeVertexAction);
	m_rightClickingMenu->addAction(m_coordEditAction);
	m_rightClickingMenu->addAction(m_reverseCenterLineAction);
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(m_conditionDataItem->clearAction());
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(m_importCenterLineAction);
	m_rightClickingMenu->addAction(m_exportCenterLineAction);
}

void GridCreatingConditionCenterAndWidth::informSelection(PreProcessorGraphicsViewInterface* v)
{
	m_polyLineController.linesActor()->GetProperty()->SetLineWidth(selectedEdgeWidth);
	m_polyLineController.pointsActor()->GetProperty()->SetPointSize(5.0);
	updateMouseCursor(v);
}

void GridCreatingConditionCenterAndWidth::informDeselection(PreProcessorGraphicsViewInterface* v)
{
	m_polyLineController.linesActor()->GetProperty()->SetLineWidth(normalEdgeWidth);
	m_polyLineController.pointsActor()->GetProperty()->SetPointSize(1.0);
	v->unsetCursor();
}

void GridCreatingConditionCenterAndWidth::viewOperationEnded(PreProcessorGraphicsViewInterface* v)
{
	updateMouseCursor(v);
}

void GridCreatingConditionCenterAndWidth::keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (! iRIC::isEnterKey(event->key())) {return;}
	if (m_mouseEventMode != meDefining) {return;}

	definePolyLine();
}

void GridCreatingConditionCenterAndWidth::keyReleaseEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/)
{}

void GridCreatingConditionCenterAndWidth::mouseDoubleClickEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (m_mouseEventMode != meDefining) {return;}

	definePolyLine();
}

void GridCreatingConditionCenterAndWidth::createSpline(vtkPoints* points, int division)
{
	if (m_mouseEventMode == meBeforeDefining || m_mouseEventMode == meDefining) {return;}
	if (! points->GetNumberOfPoints()) {return;}

	m_spline->SetPoints(points);
	m_spline->Modified();
	m_splinePoints->Initialize();

	//calculate the length of this spline
	int d = division * 5;
	// It might not work well on a too large grid
	double pre[3], dx, dy;
	std::vector<double> length(d + 1);
	length[0] = 0;

	double u[3], Pt[3], Du[9];
	u[0] = 0.0;
	m_spline->Evaluate(u, Pt, Du);
	m_splinePoints->InsertPoint(0, Pt);
	for (int i = 1; i <= d; i++) {
		u[0] = i * (1.0 / d);
		m_spline->Evaluate(u, Pt, Du);
		m_splinePoints->InsertPoint(i, Pt);

		m_splinePoints->GetPoint(i - 1, pre);
		dx = Pt[0] - pre[0];
		dy = Pt[1] - pre[1];
		length[i] = length[i - 1] + sqrt(dx * dx + dy * dy);
	}
	m_length = length[d];

	// diviede the spline at equal intervals
	m_splinePoints->Initialize();
	u[0] = 0.0;
	m_spline->Evaluate(u, Pt, Du);
	m_splinePoints->InsertPoint(0, Pt);
	vtkIdType c = 1;
	double cumulativeLength = m_length / division;
	for (int i = 0; i < d; i++) {
		if (length[i] > cumulativeLength) {
			u[0] = (i - 1) * (1.0 / d) + (1.0 / d) * ((cumulativeLength - length[i - 1]) / (length[i] - length[i - 1]));
			m_spline->Evaluate(u, Pt, Du);
			m_splinePoints->InsertPoint(c, Pt);
			c++;
			if (c == division) {
				u[0] = 1.0;
				m_spline->Evaluate(u, Pt, Du);
				m_splinePoints->InsertPoint(c, Pt);
				break;
			}
			cumulativeLength = c * m_length / division;
		}
	}
}

void GridCreatingConditionCenterAndWidth::mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	switch (m_mouseEventMode) {
	case meNormal:
	case meTranslatePrepare:
	case meMoveVertexPrepare:
	case meAddVertexPrepare:
	case meAddVertexNotPossible:
	case meRemoveVertexPrepare:
	case meRemoveVertexNotPossible:
		m_currentPoint = QPoint(event->x(), event->y());
		updateMouseEventMode();
		updateMouseCursor(v);
		break;
	case meBeforeDefining:
		// do nothing.
		break;
	case meDefining:
		// update the position of the last point.
		pushRenderCommand(new DefineNewPointCommand(false, QPoint(event->x(), event->y()), this));
		break;
	case meTranslate:
		// execute translation.
		pushUpdateShapeCommand(new MoveCommand(false, m_currentPoint, QPoint(event->x(), event->y()), this));
		m_currentPoint = QPoint(event->x(), event->y());
		break;
	case meMoveVertex:
		pushUpdateShapeCommand(new MoveVertexCommand(false, m_currentPoint, QPoint(event->x(), event->y()), m_selectedVertexId, this));
		m_currentPoint = QPoint(event->x(), event->y());
		break;
	case meAddVertex:
		 pushUpdateShapeCommand(new AddVertexCommand(false, m_selectedEdgeId, QPoint(event->x(), event->y()), this));
		break;
	case meEditVerticesDialog:
		break;
	}
}

void GridCreatingConditionCenterAndWidth::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {
		// left click
		double worldX = static_cast<double>(event->x());
		double worldY = static_cast<double>(event->y());
		v->viewportToWorld(worldX, worldY);

		switch (m_mouseEventMode) {
		case meNormal:
			// do nothing.
			break;
		case meBeforeDefining:
			// enter defining mode.
			m_mouseEventMode = meDefining;
			pushRenderCommand(new DefineNewPointCommand(true, event->pos(), this));
		case meDefining:
			pushRenderCommand(new DefineNewPointCommand(true, event->pos(), this));
			break;
		case meTranslatePrepare:
			m_mouseEventMode = meTranslate;
			m_currentPoint = event->pos();
			// push the first translation command.
			pushRenderCommand(new MoveCommand(true, event->pos(), event->pos(), this));
			break;
		case meMoveVertexPrepare:
			m_mouseEventMode = meMoveVertex;
			m_currentPoint = event->pos();
			// push the first move command.
			pushRenderCommand(new MoveVertexCommand(true, event->pos(), event->pos(), m_selectedVertexId, this));
			break;
		case meAddVertexPrepare:
			m_mouseEventMode = meAddVertex;
			pushRenderCommand(new AddVertexCommand(true, m_selectedEdgeId, event->pos(), this));
			break;
		case meAddVertexNotPossible:
			// do nothing.
			break;
		case meRemoveVertexPrepare:
			pushRenderCommand(new RemoveVertexCommand(m_selectedVertexId, this));
			break;
		case meRemoveVertexNotPossible:
			// do nothing.
			break;
		case meTranslate:
			// this should not happen.
			break;
		case meMoveVertex:
			// this should not happen.
			break;
		case meAddVertex:
			// this should not happen.
			break;
		case meEditVerticesDialog:
			break;
		}
		updateMouseCursor(v);
		updateActionStatus();
	} else if (event->button() == Qt::RightButton) {
		// right click
		m_dragStartPoint = event->pos();
	}
}

void GridCreatingConditionCenterAndWidth::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {
		switch (m_mouseEventMode) {
		case meTranslate:
		case meMoveVertex:
		case meAddVertex:
			if (m_isGridCreated) { emit gridCreated(createGrid()); }
		case meNormal:
		case meTranslatePrepare:
		case meMoveVertexPrepare:
		case meAddVertexPrepare:
		case meAddVertexNotPossible:
		case meRemoveVertexPrepare:
		case meRemoveVertexNotPossible:
			m_currentPoint = event->pos();
			updateMouseEventMode();
			updateMouseCursor(v);
			updateActionStatus();
			break;
		default:
			break;
		}
		updateMouseCursor(v);
	} else if (event->button() == Qt::RightButton) {
		if (m_mouseEventMode == meEditVerticesDialog) {return;}
		if (iRIC::isNear(m_dragStartPoint, event->pos())) {
			// show right-clicking menu.
			m_rightClickingMenu->move(event->globalPos());
			m_rightClickingMenu->show();
		}
	}
}

void GridCreatingConditionCenterAndWidth::deletePolyLine()
{
	std::vector<QPointF> emptyVec;
	m_polyLineController.setPolyLine(emptyVec);
	ZDepthRange range = dynamic_cast<PreProcessorGridCreatingConditionDataItemInterface*>(parent())->zDepthRange();
	assignActorZValues(range);

	m_mouseEventMode = meBeforeDefining;
	updateMouseCursor(graphicsView());

	m_iMax = 11;
	m_jMax = 11;
	m_width = 10;
	m_length = 0;
	updateShapeData();
	// this operation is not undo-able.
	iRICUndoStack::instance().clear();

	renderGraphicsView();
}

void GridCreatingConditionCenterAndWidth::definePolyLine()
{
	if (m_polyLineController.polyLine().size() < 2) {
		// not enough points defined!
		return;
	}
	iRICUndoStack::instance().undo();
	// finish defining the Polyline.
	pushCommand(new FinishDefiningCommand(this));
}

void GridCreatingConditionCenterAndWidth::updateMouseCursor(PreProcessorGraphicsViewInterface* v)
{
	switch (m_mouseEventMode) {
	case meNormal:
	case meAddVertexNotPossible:
	case meRemoveVertexNotPossible:
	case meEditVerticesDialog:
		v->setCursor(Qt::ArrowCursor);
		break;
	case meBeforeDefining:
	case meDefining:
		v->setCursor(Qt::CrossCursor);
		break;
	case meTranslatePrepare:
	case meMoveVertexPrepare:
		v->setCursor(Qt::OpenHandCursor);
		break;
	case meAddVertexPrepare:
		v->setCursor(m_addCursor);
		break;
	case meRemoveVertexPrepare:
		v->setCursor(m_removeCursor);
		break;
	case meTranslate:
	case meMoveVertex:
	case meAddVertex:
		v->setCursor(Qt::ClosedHandCursor);
		break;
	}
}

void GridCreatingConditionCenterAndWidth::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNode centerNode = iRIC::getChildNode(node, "CenterAndWidth");
	if (! centerNode.isNull()) {
		loadCenterAndWidthFromProjectMainFile(centerNode);
	}
}

void GridCreatingConditionCenterAndWidth::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeStartElement("CenterAndWidth");
	saveCenterAndWidthToProjectMainFile(writer);
	writer.writeEndElement();
}

void GridCreatingConditionCenterAndWidth::loadCenterAndWidthFromProjectMainFile(const QDomNode& node)
{
	m_iMax = node.toElement().attribute("imax", "11").toInt();
	m_jMax = node.toElement().attribute("jmax", "11").toInt();
	m_width = node.toElement().attribute("width", "10.0").toDouble();
	m_length = node.toElement().attribute("length", "0").toDouble();
}

void GridCreatingConditionCenterAndWidth::saveCenterAndWidthToProjectMainFile(QXmlStreamWriter& writer)
{
	QString qstr;
	writer.writeAttribute("imax", qstr.setNum(m_iMax));
	writer.writeAttribute("jmax", qstr.setNum(m_jMax));
	writer.writeAttribute("width", qstr.setNum(m_width));
	writer.writeAttribute("length", qstr.setNum(m_length));
}

void GridCreatingConditionCenterAndWidth::loadExternalData(const QString& filename)
{
	QFile f(filename);
	f.open(QIODevice::ReadOnly);
	QDataStream s(&f);
	std::vector<QPointF> v;
	auto offset = projectData()->mainfile()->offset();
	while (! s.atEnd()) {
		double a, b;
		s >> a >> b;
		v.push_back(QPointF(a - offset.x(), b - offset.y()));
	}
	f.close();
	setPolyLine(v);
	if (polyLine().size() > 0) {
		m_mouseEventMode = meNormal;
		m_isGridCreated = true;
	} else {
		m_mouseEventMode = meBeforeDefining;
	}
	updateActionStatus();
}

void GridCreatingConditionCenterAndWidth::saveExternalData(const QString& filename)
{
	QFile f(filename);
	f.open(QIODevice::WriteOnly);
	QDataStream s(&f);
	std::vector<QPointF> v = polyLine();
	int vSize = static_cast<int>(v.size());
	auto offset = projectData()->mainfile()->offset();
	for (int i = 0; i < vSize; i++) {
		s << v.at(i).x() + offset.x();
		s << v.at(i).y() + offset.y();
	}
	f.close();
}

void GridCreatingConditionCenterAndWidth::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(1);
}

void GridCreatingConditionCenterAndWidth::assignActorZValues(const ZDepthRange& range)
{
	m_polyLineController.pointsActor()->SetPosition(0, 0, range.max());
	m_polyLineController.linesActor()->SetPosition(0, 0, range.max());
	m_previewActor->SetPosition(0, 0, range.max());
}

void GridCreatingConditionCenterAndWidth::setIMax(int i)
{
	m_iMax = i;
}

void GridCreatingConditionCenterAndWidth::setJMax(int j)
{
	m_jMax = j;
}

void GridCreatingConditionCenterAndWidth::setWidth(double w)
{
	m_width = w;
}

bool GridCreatingConditionCenterAndWidth::addToolBarButtons(QToolBar* tb)
{
	tb->addAction(m_addVertexAction);
	tb->addAction(m_removeVertexAction);

	return true;
}

void GridCreatingConditionCenterAndWidth::restoreMouseEventMode()
{
	m_mouseEventMode = meNormal;
}

void GridCreatingConditionCenterAndWidth::updateMouseEventMode()
{
	double dx, dy;
	dx = m_currentPoint.x();
	dy = m_currentPoint.y();

	auto v = graphicsView();
	v->viewportToWorld(dx, dy);
	QPointF worldPos(dx, dy);

	double radius = v->stdRadius(iRIC::nearRadius());

	switch (m_mouseEventMode) {
	case meAddVertexNotPossible:
	case meAddVertexPrepare:
		if (m_polyLineController.isEdgeSelectable(worldPos, radius, &m_selectedEdgeId)) {
			m_mouseEventMode = meAddVertexPrepare;
		} else {
			m_mouseEventMode = meAddVertexNotPossible;
		}
		break;
	case meRemoveVertexNotPossible:
	case meRemoveVertexPrepare:
		if (m_polyLineController.isVertexSelectable(worldPos, radius, &m_selectedVertexId)) {
			m_mouseEventMode = meRemoveVertexPrepare;
		} else {
			m_mouseEventMode = meRemoveVertexNotPossible;
		}
		break;
	case meNormal:
	case meTranslatePrepare:
	case meMoveVertexPrepare:
	case meTranslate:
	case meMoveVertex:
	case meAddVertex:
		if (m_polyLineController.isVertexSelectable(worldPos, radius, &m_selectedVertexId)) {
			m_mouseEventMode = meMoveVertexPrepare;
		} else if (m_polyLineController.isEdgeSelectable(worldPos, radius, &m_selectedEdgeId)) {
			m_mouseEventMode = meTranslatePrepare;
		} else {
			m_mouseEventMode = meNormal;
		}
		break;
	case meBeforeDefining:
	case meDefining:
		// do nothing
		break;
	case meEditVerticesDialog:
		break;
	}
}

void GridCreatingConditionCenterAndWidth::updateActionStatus()
{
	switch (m_mouseEventMode) {
	case meBeforeDefining:
	case meDefining:
	case meTranslate:
	case meMoveVertex:
		m_addVertexAction->setDisabled(true);
		m_addVertexAction->setChecked(false);
		m_removeVertexAction->setDisabled(true);
		m_removeVertexAction->setChecked(false);
		m_coordEditAction->setEnabled(false);
		break;
	case meNormal:
	case meTranslatePrepare:
	case meMoveVertexPrepare:
		m_addVertexAction->setEnabled(true);
		m_addVertexAction->setChecked(false);
		m_removeVertexAction->setEnabled(m_polyLineController.polyData()->GetPoints()->GetNumberOfPoints() > 2);
		m_removeVertexAction->setChecked(false);
		m_coordEditAction->setEnabled(true);
		break;
	case meAddVertexPrepare:
	case meAddVertexNotPossible:
	case meAddVertex:
		m_addVertexAction->setEnabled(true);
		m_addVertexAction->setChecked(true);
		m_removeVertexAction->setDisabled(true);
		m_removeVertexAction->setChecked(false);
		m_coordEditAction->setEnabled(false);
		break;
	case meRemoveVertexPrepare:
	case meRemoveVertexNotPossible:
		m_addVertexAction->setDisabled(true);
		m_addVertexAction->setChecked(false);
		m_removeVertexAction->setEnabled(true);
		m_removeVertexAction->setChecked(true);
		m_coordEditAction->setEnabled(false);
		break;
	case meEditVerticesDialog:
		break;
	}
}

void GridCreatingConditionCenterAndWidth::pushUpdateShapeCommand(QUndoCommand* com, bool renderRedoOnly)
{
	if (renderRedoOnly) {
		pushRenderRedoOnlyCommand(new UpdateShapeCommand(com, this));
	} else {
		pushRenderCommand(new UpdateShapeCommand(com, this));
	}
}

void GridCreatingConditionCenterAndWidth::addVertexMode(bool on)
{
	if (on) {
		m_mouseEventMode = meAddVertexNotPossible;
	} else {
		m_mouseEventMode = meNormal;
	}
	updateActionStatus();
}

void GridCreatingConditionCenterAndWidth::removeVertexMode(bool on)
{
	if (on) {
		m_mouseEventMode = meRemoveVertexNotPossible;
	} else {
		m_mouseEventMode = meNormal;
	}
	updateActionStatus();
}

void GridCreatingConditionCenterAndWidth::editCoordinates()
{
	CoordinatesEditor::edit(this);
}

void GridCreatingConditionCenterAndWidth::clear()
{
	deletePolyLine();
}

void GridCreatingConditionCenterAndWidth::showInitialDialog()
{
	InformationDialog::information(preProcessorWindow(), tr("Information"), tr("Please define grid center line. Grid center line can be defined as polygonal line by mouse-clicking. Finish definining by double clicking, or pressing return key."), "gccenterandwidth_centerline");
}

void GridCreatingConditionCenterAndWidth::reverseCenterLineDirection()
{
	std::vector<QPointF> points = polyLine();
	std::vector<QPointF> revPoints;
	for (int i = static_cast<int>(points.size()) - 1; i >= 0; --i) {
		revPoints.push_back(points.at(i));
	}
	setPolyLine(revPoints);
	updateShapeData();
	renderGraphicsView();
}

void GridCreatingConditionCenterAndWidth::importCenterLine()
{
	auto polyLine = PolylineIO::importData(preProcessorWindow());
	if (polyLine.size() == 0) {return;}

	auto offset = projectData()->mainfile()->offset();
	for (QPointF& p : polyLine) {
		p.setX(p.x() - offset.x());
		p.setY(p.y() - offset.y());
	}

	setPolyLine(polyLine);
	m_mouseEventMode = meNormal;

	renderGraphicsView();
}

void GridCreatingConditionCenterAndWidth::exportCenterLine()
{
	auto l = polyLine();
	if (l.size() == 0) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Center line not defined yet"));
		return;
	}
	auto offset = projectData()->mainfile()->offset();

	for (QPointF& p : l) {
		p.setX(p.x() + offset.x());
		p.setY(p.y() + offset.y());
	}

	PolylineIO::exportData(l, preProcessorWindow());
}

void GridCreatingConditionCenterAndWidth::doApplyOffset(double x, double y)
{
	std::vector<QPointF> polyline = polyLine();
	for (int i = 0; i < polyline.size(); ++ i) {
		QPointF p = polyline.at(i);
		p.setX(p.x() - x);
		p.setY(p.y() - y);
		polyline[i] = p;
	}
	setPolyLine(polyline);
	updateShapeData();
}
