#include "gridcreatingconditionrectangularregionlonlat.h"
#include "gridcreatingconditionrectangularregionlonlatsettingdialog.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorgridcreatingconditiondataiteminterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/informationdialog.h>
#include <misc/iricundostack.h>
#include <misc/mathsupport.h>
#include <misc/xmlsupport.h>

#include <QDomElement>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QXmlStreamWriter>

#include <vtkCollectionIterator.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

GridCreatingConditionRectangularRegionLonLat::GridCreatingConditionRectangularRegionLonLat(ProjectDataItem* parent, GridCreatingConditionCreator* creator) :
	GridCreatingCondition(parent, creator),
	m_previewGrid {nullptr}
{
	m_mouseEventMode = meNormal;
	m_rightClickingMenu = nullptr;
	clear();
	graphicsView()->ResetCameraClippingRange();
}

GridCreatingConditionRectangularRegionLonLat::~GridCreatingConditionRectangularRegionLonLat()
{
	renderer()->RemoveActor(m_rectangularActor);
	renderer()->RemoveActor(m_rectangularFrameActor);
	renderer()->RemoveActor(m_previewActor);

	actorCollection()->RemoveItem(m_rectangularActor);
	actorCollection()->RemoveItem(m_rectangularFrameActor);
	actorCollection()->RemoveItem(m_previewActor);

	delete m_previewGrid;

	delete m_rightClickingMenu;
}

bool GridCreatingConditionRectangularRegionLonLat::create(QWidget* /*parent*/)
{
	auto off = offset();
	GridCreatingConditionRectangularRegionLonLatSettingDialog* dialog = new GridCreatingConditionRectangularRegionLonLatSettingDialog(this, preProcessorWindow());
	dialog->setXMin(m_xMin + off.x());
	dialog->setXMax(m_xMax + off.x());
	dialog->setYMin(m_yMin + off.y());
	dialog->setYMax(m_yMax + off.y());

	if (m_stepSize == 0) {
		m_stepSize = std::min((m_xMax - m_xMin) / 10., (m_yMax - m_yMin) / 10.);
	}
	dialog->setStepSize(m_stepSize);
	m_mouseEventMode = meCreateDialog;

	iricMainWindow()->enterModelessDialogMode();

	connect(dialog, SIGNAL(destroyed()), iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(hideTmpGrid()));
	dialog->show();
	return false;
}

bool GridCreatingConditionRectangularRegionLonLat::ready() const
{
	return true;
}

void GridCreatingConditionRectangularRegionLonLat::setupMenu()
{
	PreProcessorGridCreatingConditionDataItemInterface* p = dynamic_cast<PreProcessorGridCreatingConditionDataItemInterface*>(parent());

	m_rightClickingMenu = new QMenu();
	m_rightClickingMenu->addAction(p->createAction());
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(p->clearAction());
}

void GridCreatingConditionRectangularRegionLonLat::setupActors()
{
	if (actorCollection()->GetNumberOfItems() > 0) {
		vtkCollectionIterator* it = actorCollection()->NewIterator();
		it->GoToFirstItem();
		while (! it->IsDoneWithTraversal()) {
			vtkActor* actor = vtkActor::SafeDownCast(it->GetCurrentObject());
			renderer()->RemoveActor(actor);
			it->GoToNextItem();
		}
		actorCollection()->RemoveAllItems();
	}
	vtkSmartPointer<vtkDataSetMapper> mapper;
	mapper = vtkSmartPointer<vtkDataSetMapper>::New();

	m_rectangularActor = vtkSmartPointer<vtkActor>::New();
	m_rectangularActor->SetMapper(mapper);
	m_rectangularActor->GetProperty()->SetColor(0.7, 0.7, 0.7);
	m_rectangularActor->GetProperty()->SetOpacity(0.8);
	m_rectangularActor->GetProperty()->SetRepresentationToSurface();
	renderer()->AddActor(m_rectangularActor);

	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	m_rectangularFrameActor = vtkSmartPointer<vtkActor>::New();
	m_rectangularFrameActor->SetMapper(mapper);
	m_rectangularFrameActor->GetProperty()->SetColor(0, 0, 0);
	m_rectangularFrameActor->GetProperty()->SetRepresentationToWireframe();
	m_rectangularFrameActor->GetProperty()->SetLineWidth(2);
	renderer()->AddActor(m_rectangularFrameActor);

	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	m_previewActor = vtkSmartPointer<vtkActor>::New();
	m_previewActor->SetMapper(mapper);
	m_previewActor->GetProperty()->SetColor(0, 0, 0);
	m_previewActor->GetProperty()->SetRepresentationToWireframe();
	renderer()->AddActor(m_previewActor);

	createRectangularRegion(m_xMin, m_xMax, m_yMin, m_yMax);
	m_rectangularActor->VisibilityOff();
	m_rectangularFrameActor->VisibilityOff();
	m_previewActor->VisibilityOff();

	updateVisibilityWithoutRendering();
}

void GridCreatingConditionRectangularRegionLonLat::mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (m_mouseEventMode != meDragging) {return;}
	if (event->modifiers() == Qt::ControlModifier) { return; }

	m_endPoint = QPoint(event->x(), event->y());
	createRectangularRegionFromMouse();
}

void GridCreatingConditionRectangularRegionLonLat::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (event->button() == Qt::LeftButton) {
		if (m_mouseEventMode == meNormal) {
			m_startPoint = QPoint(event->x(), event->y());
			m_mouseEventMode = meDragging;
		}
	}
	if (event->button() == Qt::RightButton) {
		m_dragStartPoint = QPoint(event->x(), event->y());
	}
}

void GridCreatingConditionRectangularRegionLonLat::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {
		if (m_mouseEventMode == meDragging) {
			m_endPoint = QPoint(event->x(), event->y());
			createRectangularRegionFromMouse();
			m_mouseEventMode = meNormal;
		}
	}
	if (event->button() == Qt::RightButton) {
		if (iRIC::isNear(m_dragStartPoint, event->pos())) {
			// show right-clicking menu.
			m_rightClickingMenu->move(event->globalPos());
			m_rightClickingMenu->show();
		}
	}
	updateMouseCursor(v);
}

void GridCreatingConditionRectangularRegionLonLat::createRectangularRegionFromMouse()
{
	double worldPressX = m_startPoint.x();
	double worldPressY = m_startPoint.y();
	double worldNewX = m_endPoint.x();
	double worldNewY = m_endPoint.y();

	graphicsView()->viewportToWorld(worldPressX, worldPressY);
	graphicsView()->viewportToWorld(worldNewX, worldNewY);

	m_xMin = std::min(worldPressX, worldNewX);
	m_xMax = std::max(worldPressX, worldNewX);
	m_yMin = std::min(worldPressY, worldNewY);
	m_yMax = std::max(worldPressY, worldNewY);

	createRectangularRegion(m_xMin, m_xMax, m_yMin, m_yMax);
	renderGraphicsView();
}

void GridCreatingConditionRectangularRegionLonLat::createRectangularRegion(double xmin, double xmax, double ymin, double ymax)
{
	vtkStructuredGrid* grid = vtkStructuredGrid::New();
	grid->SetDimensions(2, 2, 1);
	vtkPoints* points = vtkPoints::New();
	points->SetDataTypeToDouble();
	points->InsertPoint(0, xmin, ymin, 0.);
	points->InsertPoint(1, xmax, ymin, 0.);
	points->InsertPoint(2, xmin, ymax, 0.);
	points->InsertPoint(3, xmax, ymax, 0.);
	grid->SetPoints(points);
	vtkDataSetMapper* mapper;
	mapper = vtkDataSetMapper::SafeDownCast(m_rectangularActor->GetMapper());
	mapper->SetInputData(grid);
	mapper = vtkDataSetMapper::SafeDownCast(m_rectangularFrameActor->GetMapper());
	mapper->SetInputData(grid);
	points->Delete();
	grid->Delete();

	if (xmin != xmax && ymin != ymax) {
		actorCollection()->AddItem(m_rectangularActor);
		actorCollection()->AddItem(m_rectangularFrameActor);
	}
	updateVisibilityWithoutRendering();
}

void GridCreatingConditionRectangularRegionLonLat::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(2);
}

void GridCreatingConditionRectangularRegionLonLat::assignActorZValues(const ZDepthRange& range)
{
	m_rectangularActor->SetPosition(0, 0, range.min());
	m_previewActor->SetPosition(0, 0, range.min());
	m_rectangularFrameActor->SetPosition(0, 0, range.max());
}

void GridCreatingConditionRectangularRegionLonLat::doLoadFromProjectMainFile(const QDomNode& node)
{
	auto off = offset();
	QDomElement elem = node.toElement();
	m_xMin = iRIC::getDoubleAttribute(elem, "xMin") - off.x();
	m_xMax = iRIC::getDoubleAttribute(elem, "xMax") - off.x();
	m_yMin = iRIC::getDoubleAttribute(elem, "yMin") - off.y();
	m_yMax = iRIC::getDoubleAttribute(elem, "yMax") - off.y();
	m_stepSize = iRIC::getDoubleAttribute(elem, "stepSize");
}

void GridCreatingConditionRectangularRegionLonLat::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	auto off = offset();
	iRIC::setDoubleAttribute(writer, "xMin", m_xMin + off.x());
	iRIC::setDoubleAttribute(writer, "xMax", m_xMax + off.x());
	iRIC::setDoubleAttribute(writer, "yMin", m_yMin + off.y());
	iRIC::setDoubleAttribute(writer, "yMax", m_yMax + off.y());
	iRIC::setDoubleAttribute(writer, "stepSize", m_stepSize);
}

void GridCreatingConditionRectangularRegionLonLat::doApplyOffset(double x, double y)
{
	m_xMin -= x;
	m_xMax -= x;
	m_yMin -= y;
	m_yMax -= y;

	createRectangularRegion(m_xMin, m_xMax, m_yMin, m_yMax);
}

bool GridCreatingConditionRectangularRegionLonLat::createGrid(double xmin, double xmax, double ymin, double ymax, double step)
{
	auto off = offset();
	xmin -= off.x();
	xmax -= off.x();
	ymin -= off.y();
	ymax -= off.y();
	Structured2DGrid* grid = createGridInner(xmin, xmax, ymin, ymax, step);
	if (grid == nullptr) {return false;}
	m_xMin = xmin;
	m_xMax = xmax;
	m_yMin = ymin;
	m_yMax = ymax;
	m_stepSize = step;
	emit gridCreated(grid);
	iRICUndoStack::instance().clear();
	return true;
}

void GridCreatingConditionRectangularRegionLonLat::previewGrid(double xmin, double xmax, double ymin, double ymax, double step)
{
	auto off = offset();
	xmin -= off.x();
	xmax -= off.x();
	ymin -= off.y();
	ymax -= off.y();
	createRectangularRegion(xmin, xmax, ymin, ymax);
	Structured2DGrid* grid = createGridInner(xmin, xmax, ymin, ymax, step);
	if (grid == nullptr) {return;}

	if (m_previewGrid != nullptr) {
		delete m_previewGrid;
	}
	m_previewGrid = grid;
	actorCollection()->RemoveItem(m_previewActor);

	vtkStructuredGrid* sgrid = m_previewGrid->vtkGrid();
	m_previewActor->GetMapper()->SetInputDataObject(sgrid);
	actorCollection()->AddItem(m_previewActor);

	updateVisibility();
}

Structured2DGrid* GridCreatingConditionRectangularRegionLonLat::createGridInner(double xmin, double xmax, double ymin, double ymax, double stepSize)
{
	Structured2DGrid* grid = new Structured2DGrid(nullptr);
	PreProcessorGridTypeDataItemInterface* gt = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(m_conditionDataItem->parent()->parent());
	gt->gridType()->buildGridAttributes(grid);

	unsigned int imax = floor((xmax - xmin) / stepSize);
	unsigned int jmax = floor((ymax - ymin) / stepSize);
	if (imax == 0 || jmax == 0) {return nullptr;}

	imax += 1;
	jmax += 1;

	if (imax * jmax > MAXGRIDSIZE) {
		QMessageBox::warning(dataModel()->mainWindow(), tr("Warning"), tr("The maximum number of grid nodes is %1.").arg(MAXGRIDSIZE));
		return nullptr;
	}
	grid->setDimensions(imax, jmax);
	vtkPoints* points = vtkPoints::New();
	points->SetDataTypeToDouble();
	points->Allocate((imax - 1) * (jmax - 1));
	for (unsigned int j = 0; j < jmax; j++) {
		for (unsigned int i = 0; i < imax; i++) {
			points->InsertPoint(imax * j + i, xmin + stepSize * i, ymin + stepSize * j, 0.);
		}
	}
	grid->vtkGrid()->SetPoints(points);

	// allocate memory for all grid related conditions.
	for (GridAttributeContainer* c : grid->gridAttributes()) {
		c->allocate();
	}
	grid->setModified();
	return grid;
}

void GridCreatingConditionRectangularRegionLonLat::hideTmpGrid()
{
	m_mouseEventMode = meNormal;

	m_previewActor->VisibilityOff();
	actorCollection()->RemoveItem(m_previewActor);

	delete m_previewGrid;
	m_previewGrid = nullptr;
}

void GridCreatingConditionRectangularRegionLonLat::informSelection(PreProcessorGraphicsViewInterface* v)
{
	actorCollection()->AddItem(m_rectangularActor);
	actorCollection()->AddItem(m_rectangularFrameActor);
	updateMouseCursor(v);
	updateVisibility();
}

void GridCreatingConditionRectangularRegionLonLat::informDeselection(PreProcessorGraphicsViewInterface* v)
{
	m_rectangularFrameActor->VisibilityOff();
	m_rectangularActor->VisibilityOff();
	v->unsetCursor();
	actorCollection()->RemoveAllItems();
}

void GridCreatingConditionRectangularRegionLonLat::clear()
{
	m_xMin = 0;
	m_xMax = 0;
	m_yMin = 0;
	m_yMax = 0;
	m_stepSize = 0;

	if (m_rectangularActor != nullptr) {
		m_rectangularActor->VisibilityOff();
		m_rectangularFrameActor->VisibilityOff();
	}
}

void GridCreatingConditionRectangularRegionLonLat::updateMouseCursor(PreProcessorGraphicsViewInterface* v)
{
	v->setCursor(Qt::CrossCursor);
}

void GridCreatingConditionRectangularRegionLonLat::showInitialDialog()
{
	InformationDialog::information(preProcessorWindow(), GridCreatingConditionRectangularRegionLonLat::tr("Information"), GridCreatingConditionRectangularRegionLonLat::tr("Please define grid region by mouse-dragging."), "gcrectangular_region");
}
