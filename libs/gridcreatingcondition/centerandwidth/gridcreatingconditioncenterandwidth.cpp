#include "gridcreatingconditioncenterandwidth.h"
#include "gridcreatingconditioncenterandwidthdialog.h"
#include "private/gridcreatingconditioncenterandwidth_addvertexcommand.h"
#include "private/gridcreatingconditioncenterandwidth_coordinateseditor.h"
#include "private/gridcreatingconditioncenterandwidth_pushvertexcommand.h"
#include "private/gridcreatingconditioncenterandwidth_finishdefiningcommand.h"
#include "private/gridcreatingconditioncenterandwidth_impl.h"
#include "private/gridcreatingconditioncenterandwidth_movecommand.h"
#include "private/gridcreatingconditioncenterandwidth_movevertexcommand.h"
#include "private/gridcreatingconditioncenterandwidth_removevertexcommand.h"
#include "private/gridcreatingconditioncenterandwidth_updateshapecommand.h"

#include <geoio/polylineio.h>
#include <guibase/vtktextpropertysettingdialog.h>
#include <guicore/base/iricmainwindowi.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/base/preprocessorgraphicsviewi.h>
#include <guicore/pre/base/preprocessorgridcreatingconditiondataitemi.h>
#include <guicore/pre/base/preprocessorgridtypedataitemi.h>
#include <guicore/pre/base/preprocessorwindowi.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/pre/gridcond/container/gridattributerealcontainer.h>
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

void setupLabelActor(vtkLabel2DActor* actor)
{
	actor->setLabelPosition(vtkLabel2DActor::lpMiddleRight);
}

} // namespace

GridCreatingConditionCenterAndWidth::GridCreatingConditionCenterAndWidth(ProjectDataItem* parent, GridCreatingConditionCreator* creator) :
	GridCreatingCondition(parent, creator),
	impl {new Impl {this}}
{
	impl->m_upstreamActor.setLabel("Upstream");
	setupLabelActor(&impl->m_upstreamActor);

	impl->m_downstreamActor.setLabel("Downstream");
	setupLabelActor(&impl->m_downstreamActor);

	impl->updateActionStatus();
}

GridCreatingConditionCenterAndWidth::~GridCreatingConditionCenterAndWidth()
{
	auto r = renderer();
	r->RemoveActor(impl->m_previewActor);
	r->RemoveActor(impl->m_polyLineController.pointsActor());
	r->RemoveActor(impl->m_polyLineController.linesActor());
	r->RemoveActor2D(impl->m_upstreamActor.actor());
	r->RemoveActor2D(impl->m_downstreamActor.actor());

	delete impl->m_rightClickingMenu;
	delete impl;
}

std::vector<QPointF> GridCreatingConditionCenterAndWidth::polyLine()
{
	return impl->m_polyLineController.polyLine();
}

void GridCreatingConditionCenterAndWidth::setPolyLine(const std::vector<QPointF>& polyline)
{
	impl->m_polyLineController.setPolyLine(polyline);
}

void GridCreatingConditionCenterAndWidth::updateShapeData()
{
	auto col = actor2DCollection();

	impl->m_upstreamActor.actor()->VisibilityOff();
	impl->m_downstreamActor.actor()->VisibilityOff();
	col->RemoveItem(impl->m_upstreamActor.actor());
	col->RemoveItem(impl->m_downstreamActor.actor());

	auto polyLine = impl->m_polyLineController.polyLine();
	if (polyLine.size() < 2) {return;}

	impl->m_upstreamActor.setPosition(polyLine.at(0));
	impl->m_downstreamActor.setPosition(polyLine.at(polyLine.size() - 1));
	impl->m_setting.upDownText.applySetting(impl->m_upstreamActor.labelTextProperty());
	impl->m_setting.upDownText.applySetting(impl->m_downstreamActor.labelTextProperty());

	col->AddItem(impl->m_upstreamActor.actor());
	col->AddItem(impl->m_downstreamActor.actor());

	updateVisibilityWithoutRendering();
}

bool GridCreatingConditionCenterAndWidth::create(QWidget* parent)
{
	if (impl->m_mouseEventMode == Impl::MouseEventMode::BeforeDefining || impl->m_mouseEventMode == Impl::MouseEventMode::Defining) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Grid creating condition definition not finished yet."));
		return false;
	}
	if (polyLine().size() < 2) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("Grid center line is not defined yet. Please click in the main region to define the grid center line."));
		return false;
	}

	createSpline(impl->m_polyLineController.polyData()->GetPoints(), impl->m_setting.iMax);
	showDialog(parent);
	if (! impl->m_isAccepted) {return false;}

	auto grid = createGrid();
	if (grid == nullptr) {return false;}
	impl->m_isGridCreated = true;

	emit gridCreated(grid);
	return true;
}

v4InputGrid* GridCreatingConditionCenterAndWidth::createGrid()
{
	if (impl->m_setting.iMax * impl->m_setting.jMax > MAXGRIDSIZE) {
		QMessageBox::warning(preProcessorWindow(), tr("Warning"), tr("The maximum number of grid nodes is %1.").arg(MAXGRIDSIZE));
		return nullptr;
	}
	createSpline(impl->m_polyLineController.polyData()->GetPoints(), impl->m_setting.iMax - 1);

	auto grid = new v4Structured2dGrid();
	grid->setDimensions(impl->m_setting.iMax, impl->m_setting.jMax);

	for (int j = 0; j < impl->m_setting.jMax; j++) {
		for (int i = 0; i < impl->m_setting.iMax; i++) {
			double p1[3], p2[3];
			if (i == 0) {
				impl->m_splinePoints->GetPoint(i, p1);
				impl->m_splinePoints->GetPoint(i + 1, p2);
			} else if (i == impl->m_setting.iMax - 1) {
				impl->m_splinePoints->GetPoint(i, p1);
				impl->m_splinePoints->GetPoint(i - 1, p2);
			} else {
				impl->m_splinePoints->GetPoint(i - 1, p1);
				impl->m_splinePoints->GetPoint(i + 1, p2);
			}
			double dx = p2[0] - p1[0];
			double dy = p2[1] - p1[1];
			double s = sqrt(dx * dx + dy * dy);
			dx = (impl->m_setting.width / (impl->m_setting.jMax - 1)) * dx / s;
			dy = (impl->m_setting.width / (impl->m_setting.jMax - 1)) * dy / s;
			int center = (impl->m_setting.jMax - 1) / 2;
			if (i == 0) {
				grid->setPoint2d(i, j, QPointF(p1[0] + ((- 1) * center + j) * (- dy), p1[1] + ((- 1) * center + j) * dx));
			} else if (i == impl->m_setting.iMax - 1) {
				grid->setPoint2d(i, j, QPointF(p1[0] + (center - j) * (- dy), p1[1] + (center - j) * dx));
			} else {
				impl->m_splinePoints->GetPoint(i, p1);
				grid->setPoint2d(i, j, QPointF(p1[0] + ((- 1) * center + j) * (- dy), p1[1] + ((- 1) * center + j) * dx));
			}
		}
	}

	auto gt = dynamic_cast<PreProcessorGridTypeDataItemI*>(m_conditionDataItem->parent()->parent());
	auto ret = new v4InputGrid(gt->gridType(), grid);
	gt->gridType()->buildGridAttributes(ret);

	ret->allocateAttributes();
	return ret;
}

void GridCreatingConditionCenterAndWidth::showDialog(QWidget* parent)
{
	GridCreatingConditionCenterAndWidthDialog* dialog = new GridCreatingConditionCenterAndWidthDialog(parent);
	connect(dialog, &GridCreatingConditionCenterAndWidthDialog::applied, this, &GridCreatingConditionCenterAndWidth::handleDialogApplied);

	dialog->setLength(impl->m_setting.length);
	dialog->setWidth(impl->m_setting.width);
	dialog->setIMax(impl->m_setting.iMax);
	dialog->setJMax(impl->m_setting.jMax);
	impl->m_oldIMax = impl->m_setting.iMax;
	impl->m_oldJMax = impl->m_setting.jMax;
	impl->m_oldWidth = impl->m_setting.width;

	int result = dialog->exec();
	if (result == QDialog::Accepted) {
		handleDialogAccepted(dialog);
		impl->m_isAccepted = true;
	} else {
		handleDialogRejected(dialog);
		impl->m_isAccepted = false;
	}
	delete dialog;
}

void GridCreatingConditionCenterAndWidth::handleDialogApplied(QDialog* d)
{
	auto dialog = dynamic_cast<GridCreatingConditionCenterAndWidthDialog*>(d);

	setIMax(dialog->iMax());
	setJMax(dialog->jMax());
	setWidth(dialog->width());

	createSpline(impl->m_polyLineController.polyData()->GetPoints(), impl->m_setting.iMax - 1);

	auto g = createGrid();
	if (g == nullptr) {return;}
	if (impl->m_previewGrid != nullptr) {delete impl->m_previewGrid;}
	impl->m_previewGrid = g;

	impl->m_previewMapper->SetInputData(impl->m_previewGrid->grid()->vtkData()->data());
	impl->m_previewActor->VisibilityOn();
	renderGraphicsView();
}

void GridCreatingConditionCenterAndWidth::handleDialogAccepted(QDialog* d)
{
	auto dialog = dynamic_cast<GridCreatingConditionCenterAndWidthDialog*>(d);

	setIMax(dialog->iMax());
	setJMax(dialog->jMax());
	setWidth(dialog->width());
	createSpline(impl->m_polyLineController.polyData()->GetPoints(), impl->m_setting.iMax - 1);

	if (impl->m_previewGrid != nullptr) {
		delete impl->m_previewGrid;
		impl->m_previewGrid = nullptr;
	}
	impl->m_previewActor->VisibilityOff();
}

void GridCreatingConditionCenterAndWidth::handleDialogRejected(QDialog* /*d*/)
{
	setIMax(impl->m_oldIMax);
	setJMax(impl->m_oldJMax);
	setWidth(impl->m_oldWidth);
	impl->m_splinePoints->Initialize();

	if (impl->m_previewGrid != nullptr) {
		delete impl->m_previewGrid;
		impl->m_previewGrid = nullptr;
	}
	impl->m_previewActor->VisibilityOff();

	renderGraphicsView();
}

bool GridCreatingConditionCenterAndWidth::ready() const
{
	return true;
}

void GridCreatingConditionCenterAndWidth::setupActors()
{
	impl->m_polyLineController.linesActor()->GetProperty()->SetLineWidth(2);

	auto r = renderer();
	r->AddActor(impl->m_polyLineController.linesActor());
	r->AddActor(impl->m_polyLineController.pointsActor());
	r->AddActor(impl->m_previewActor);
	r->AddActor2D(impl->m_upstreamActor.actor());
	r->AddActor2D(impl->m_downstreamActor.actor());

	impl->m_previewActor->VisibilityOff();

	actorCollection()->AddItem(impl->m_polyLineController.linesActor());
	actorCollection()->AddItem(impl->m_polyLineController.pointsActor());
	actor2DCollection()->AddItem(impl->m_upstreamActor.actor());
	actor2DCollection()->AddItem(impl->m_downstreamActor.actor());

	updateShapeData();
}

void GridCreatingConditionCenterAndWidth::setupMenu()
{
	m_menu->addAction(impl->m_addVertexAction);
	m_menu->addAction(impl->m_removeVertexAction);
	m_menu->addAction(impl->m_coordEditAction);
	m_menu->addAction(impl->m_reverseCenterLineAction);
	m_menu->addSeparator();
	m_menu->addAction(impl->m_importCenterLineAction);
	m_menu->addAction(impl->m_exportCenterLineAction);

	impl->m_rightClickingMenu->addAction(m_conditionDataItem->createAction());
	impl->m_rightClickingMenu->addSeparator();
	impl->m_rightClickingMenu->addAction(impl->m_addVertexAction);
	impl->m_rightClickingMenu->addAction(impl->m_removeVertexAction);
	impl->m_rightClickingMenu->addAction(impl->m_coordEditAction);
	impl->m_rightClickingMenu->addAction(impl->m_reverseCenterLineAction);
	impl->m_rightClickingMenu->addSeparator();
	impl->m_rightClickingMenu->addAction(m_conditionDataItem->clearAction());
	impl->m_rightClickingMenu->addSeparator();
	impl->m_rightClickingMenu->addAction(impl->m_importCenterLineAction);
	impl->m_rightClickingMenu->addAction(impl->m_exportCenterLineAction);
}

void GridCreatingConditionCenterAndWidth::informSelection(PreProcessorGraphicsViewI* v)
{
	impl->m_polyLineController.linesActor()->GetProperty()->SetLineWidth(selectedEdgeWidth);
	impl->m_polyLineController.pointsActor()->GetProperty()->SetPointSize(5.0);
	impl->updateMouseCursor(v);
}

void GridCreatingConditionCenterAndWidth::informDeselection(PreProcessorGraphicsViewI* v)
{
	impl->m_polyLineController.linesActor()->GetProperty()->SetLineWidth(normalEdgeWidth);
	impl->m_polyLineController.pointsActor()->GetProperty()->SetPointSize(1.0);
	v->unsetCursor();
}

void GridCreatingConditionCenterAndWidth::viewOperationEnded(PreProcessorGraphicsViewI* v)
{
	impl->updateMouseCursor(v);
}

void GridCreatingConditionCenterAndWidth::keyPressEvent(QKeyEvent* event, PreProcessorGraphicsViewI* /*v*/)
{
	if (! iRIC::isEnterKey(event->key())) {return;}
	if (impl->m_mouseEventMode != Impl::MouseEventMode::Defining) {return;}

	definePolyLine();
}

void GridCreatingConditionCenterAndWidth::keyReleaseEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewI* /*v*/)
{}

void GridCreatingConditionCenterAndWidth::mouseDoubleClickEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewI* /*v*/)
{
	if (impl->m_mouseEventMode != Impl::MouseEventMode::Defining) {return;}

	definePolyLine();
}

void GridCreatingConditionCenterAndWidth::createSpline(vtkPoints* points, int division)
{
	if (impl->m_mouseEventMode == Impl::MouseEventMode::BeforeDefining || impl->m_mouseEventMode == Impl::MouseEventMode::Defining) {return;}
	if (! points->GetNumberOfPoints()) {return;}

	impl->m_spline->SetPoints(points);
	impl->m_spline->Modified();
	impl->m_splinePoints->Initialize();

	//calculate the length of this spline
	int d = division * 5;
	// It might not work well on a too large grid
	double pre[3], dx, dy;
	std::vector<double> length(d + 1);
	length[0] = 0;

	double u[3], Pt[3], Du[9];
	u[0] = 0.0;
	impl->m_spline->Evaluate(u, Pt, Du);
	impl->m_splinePoints->InsertPoint(0, Pt);
	for (int i = 1; i <= d; i++) {
		u[0] = i * (1.0 / d);
		impl->m_spline->Evaluate(u, Pt, Du);
		impl->m_splinePoints->InsertPoint(i, Pt);

		impl->m_splinePoints->GetPoint(i - 1, pre);
		dx = Pt[0] - pre[0];
		dy = Pt[1] - pre[1];
		length[i] = length[i - 1] + sqrt(dx * dx + dy * dy);
	}
	impl->m_setting.length = length[d];

	// diviede the spline at equal intervals
	impl->m_splinePoints->Initialize();
	u[0] = 0.0;
	impl->m_spline->Evaluate(u, Pt, Du);
	impl->m_splinePoints->InsertPoint(0, Pt);
	vtkIdType c = 1;
	double cumulativeLength = impl->m_setting.length / division;
	for (int i = 0; i < d; i++) {
		if (length[i] > cumulativeLength) {
			u[0] = (i - 1) * (1.0 / d) + (1.0 / d) * ((cumulativeLength - length[i - 1]) / (length[i] - length[i - 1]));
			impl->m_spline->Evaluate(u, Pt, Du);
			impl->m_splinePoints->InsertPoint(c, Pt);
			c++;
			if (c == division) {
				u[0] = 1.0;
				impl->m_spline->Evaluate(u, Pt, Du);
				impl->m_splinePoints->InsertPoint(c, Pt);
				break;
			}
			cumulativeLength = c * impl->m_setting.length / division;
		}
	}
}

void GridCreatingConditionCenterAndWidth::mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v)
{
	switch (impl->m_mouseEventMode) {
	case Impl::MouseEventMode::Normal:
	case Impl::MouseEventMode::TranslatePrepare:
	case Impl::MouseEventMode::MoveVertexPrepare:
	case Impl::MouseEventMode::AddVertexPrepare:
	case Impl::MouseEventMode::AddVertexNotPossible:
	case Impl::MouseEventMode::RemoveVertexPrepare:
	case Impl::MouseEventMode::RemoveVertexNotPossible:
		impl->updateMouseEventMode(event->pos(), v);
		impl->updateMouseCursor(v);
		break;
	case Impl::MouseEventMode::BeforeDefining:
		// do nothing.
		break;
	case Impl::MouseEventMode::Defining:
		// update the position of the last point.
		pushUpdateShapeCommand(new PushVertexCommand(false, event->pos(), this));
		break;
	case Impl::MouseEventMode::Translate:
		// execute translation.
		pushUpdateShapeCommand(new MoveCommand(false, impl->m_previousPoint, event->pos(), this));
		impl->m_previousPoint = QPoint(event->x(), event->y());
		break;
	case Impl::MouseEventMode::MoveVertex:
		pushUpdateShapeCommand(new MoveVertexCommand(false, impl->m_previousPoint, event->pos(), impl->m_selectedVertexId, this));
		impl->m_previousPoint = QPoint(event->x(), event->y());
		break;
	case Impl::MouseEventMode::AddVertex:
		 pushUpdateShapeCommand(new AddVertexCommand(false, impl->m_selectedEdgeId, event->pos(), this));
		break;
	case Impl::MouseEventMode::EditVerticesDialog:
		break;
	}
}

void GridCreatingConditionCenterAndWidth::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v)
{
	if (event->button() == Qt::LeftButton) {
		switch (impl->m_mouseEventMode) {
		case Impl::MouseEventMode::Normal:
			// do nothing.
			break;
		case Impl::MouseEventMode::BeforeDefining:
			// enter defining mode.
			impl->m_mouseEventMode = Impl::MouseEventMode::Defining;
			pushUpdateShapeCommand(new PushVertexCommand(true, event->pos(), this));
			break;
		case Impl::MouseEventMode::Defining:
			pushUpdateShapeCommand(new PushVertexCommand(true, event->pos(), this));
			break;
		case Impl::MouseEventMode::TranslatePrepare:
			impl->m_mouseEventMode = Impl::MouseEventMode::Translate;
			impl->m_previousPoint = event->pos();
			// push the first translation command.
			pushUpdateShapeCommand(new MoveCommand(true, event->pos(), event->pos(), this));
			break;
		case Impl::MouseEventMode::MoveVertexPrepare:
			impl->m_mouseEventMode = Impl::MouseEventMode::MoveVertex;
			impl->m_previousPoint = event->pos();
			// push the first move command.
			pushUpdateShapeCommand(new MoveVertexCommand(true, event->pos(), event->pos(), impl->m_selectedVertexId, this));
			break;
		case Impl::MouseEventMode::AddVertexPrepare:
			impl->m_mouseEventMode = Impl::MouseEventMode::AddVertex;
			pushUpdateShapeCommand(new AddVertexCommand(true, impl->m_selectedEdgeId, event->pos(), this));
			break;
		case Impl::MouseEventMode::AddVertexNotPossible:
			// do nothing.
			break;
		case Impl::MouseEventMode::RemoveVertexPrepare:
			pushUpdateShapeCommand(new RemoveVertexCommand(impl->m_selectedVertexId, this));
			break;
		case Impl::MouseEventMode::RemoveVertexNotPossible:
			// do nothing.
			break;
		case Impl::MouseEventMode::Translate:
			// this should not happen.
			break;
		case Impl::MouseEventMode::MoveVertex:
			// this should not happen.
			break;
		case Impl::MouseEventMode::AddVertex:
			// this should not happen.
			break;
		case Impl::MouseEventMode::EditVerticesDialog:
			break;
		}
		impl->updateMouseCursor(v);
		impl->updateActionStatus();
	} else if (event->button() == Qt::RightButton) {
		// right click
		impl->m_dragStartPoint = event->pos();
	}
}

void GridCreatingConditionCenterAndWidth::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewI* v)
{
	if (event->button() == Qt::LeftButton) {
		switch (impl->m_mouseEventMode) {
		case Impl::MouseEventMode::Translate:
		case Impl::MouseEventMode::MoveVertex:
		case Impl::MouseEventMode::AddVertex:
			if (impl->m_isGridCreated) { emit gridCreated(createGrid()); }
		case Impl::MouseEventMode::Normal:
		case Impl::MouseEventMode::TranslatePrepare:
		case Impl::MouseEventMode::MoveVertexPrepare:
		case Impl::MouseEventMode::AddVertexPrepare:
		case Impl::MouseEventMode::AddVertexNotPossible:
		case Impl::MouseEventMode::RemoveVertexPrepare:
		case Impl::MouseEventMode::RemoveVertexNotPossible:
			impl->updateMouseEventMode(event->pos(), v);
			impl->updateMouseCursor(v);
			impl->updateActionStatus();
			break;
		default:
			break;
		}
		impl->updateMouseCursor(v);
	} else if (event->button() == Qt::RightButton) {
		if (impl->m_mouseEventMode == Impl::MouseEventMode::EditVerticesDialog) {return;}
		if (iRIC::isNear(impl->m_dragStartPoint, event->pos())) {
			// show right-clicking menu.
			impl->m_rightClickingMenu->move(event->globalPos());
			impl->m_rightClickingMenu->show();
		}
	}
}

void GridCreatingConditionCenterAndWidth::deletePolyLine()
{
	std::vector<QPointF> emptyVec;
	impl->m_polyLineController.setPolyLine(emptyVec);
	ZDepthRange range = dynamic_cast<PreProcessorGridCreatingConditionDataItemI*>(parent())->zDepthRange();
	assignActorZValues(range);

	impl->m_mouseEventMode = Impl::MouseEventMode::BeforeDefining;
	impl->updateMouseCursor(graphicsView());

	impl->m_setting.iMax = 11;
	impl->m_setting.jMax = 11;
	impl->m_setting.width = 10;
	impl->m_setting.length = 0;
	updateShapeData();
	// this operation is not undo-able.
	iRICUndoStack::instance().clear();

	renderGraphicsView();
}

void GridCreatingConditionCenterAndWidth::definePolyLine()
{
	if (impl->m_polyLineController.polyLine().size() < 2) {
		// not enough points defined!
		return;
	}
	iRICUndoStack::instance().undo();
	// finish defining the Polyline.
	pushCommand(new FinishDefiningCommand(this));
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
	impl->m_setting.load(node);
}

void GridCreatingConditionCenterAndWidth::saveCenterAndWidthToProjectMainFile(QXmlStreamWriter& writer)
{
	impl->m_setting.save(writer);
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
		impl->m_mouseEventMode = Impl::MouseEventMode::Normal;
		impl->m_isGridCreated = true;
	} else {
		impl->m_mouseEventMode = Impl::MouseEventMode::BeforeDefining;
	}
	impl->updateActionStatus();
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
	impl->m_polyLineController.pointsActor()->SetPosition(0, 0, range.max());
	impl->m_polyLineController.linesActor()->SetPosition(0, 0, range.max());
	impl->m_previewActor->SetPosition(0, 0, range.max());
}

QDialog* GridCreatingConditionCenterAndWidth::propertyDialog(QWidget* parent)
{
	return nullptr;
	// temporarily hidden
	auto dialog = new vtkTextPropertySettingDialog(parent);
	dialog->setWindowTitle(tr("Display Setting"));
	dialog->setSetting(impl->m_setting.upDownText);

	return dialog;
}

void GridCreatingConditionCenterAndWidth::handlePropertyDialogAccepted(QDialog* propDialog)
{
	auto dialog = dynamic_cast<vtkTextPropertySettingDialog*> (propDialog);
	impl->m_setting.upDownText = dialog->setting();

	updateShapeData();
	renderGraphicsView();
}

void GridCreatingConditionCenterAndWidth::setIMax(int i)
{
	impl->m_setting.iMax = i;
}

void GridCreatingConditionCenterAndWidth::setJMax(int j)
{
	impl->m_setting.jMax = j;
}

void GridCreatingConditionCenterAndWidth::setWidth(double w)
{
	impl->m_setting.width = w;
}

int GridCreatingConditionCenterAndWidth::iMax() const
{
	return impl->m_setting.iMax;
}

int GridCreatingConditionCenterAndWidth::jMax() const
{
	return impl->m_setting.jMax;
}

double GridCreatingConditionCenterAndWidth::width() const
{
	return impl->m_setting.width;
}

double GridCreatingConditionCenterAndWidth::length() const
{
	return impl->m_setting.length;
}

bool GridCreatingConditionCenterAndWidth::addToolBarButtons(QToolBar* tb)
{
	tb->addAction(impl->m_addVertexAction);
	tb->addAction(impl->m_removeVertexAction);

	return true;
}

void GridCreatingConditionCenterAndWidth::restoreMouseEventMode()
{
	impl->m_mouseEventMode = Impl::MouseEventMode::Normal;
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
		impl->m_mouseEventMode = Impl::MouseEventMode::AddVertexNotPossible;
	} else {
		impl->m_mouseEventMode = Impl::MouseEventMode::Normal;
	}
	impl->updateActionStatus();
}

void GridCreatingConditionCenterAndWidth::removeVertexMode(bool on)
{
	if (on) {
		impl->m_mouseEventMode = Impl::MouseEventMode::RemoveVertexNotPossible;
	} else {
		impl->m_mouseEventMode = Impl::MouseEventMode::Normal;
	}
	impl->updateActionStatus();
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
	impl->m_mouseEventMode = Impl::MouseEventMode::Normal;

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
