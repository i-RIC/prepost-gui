#include "gridcreatingconditionrectangularregion.h"
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/base/gridrelatedconditioncontainer.h>
#include <guicore/pre/base/PreProcessorGridTypeDataIteminterface.h>
//#include "pre/datamodel/preprocessorgridandgridcreatingconditiondataitem.h"
#include <guicore/pre/base/preprocessorgridcreatingconditiondataiteminterface.h>
//#include "pre/datamodel/preprocessorgriddataitem.h"
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include "gridcreatingconditionrectangularregionsettingdialog.h"
#include <guicore/base/iricmainwindowinterface.h>
#include <misc/informationdialog.h>

#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <misc/iricundostack.h>

#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkCollectionIterator.h>

GridCreatingConditionRectangularRegion::GridCreatingConditionRectangularRegion(ProjectDataItem* parent, GridCreatingConditionCreator* creator)
	: GridCreatingCondition(parent, creator)
{
	m_mouseEventMode = meNormal;
	m_rightClickingMenu = 0;
	clear();
	graphicsView()->ResetCameraClippingRange();

}

GridCreatingConditionRectangularRegion::~GridCreatingConditionRectangularRegion()
{
	renderer()->RemoveActor(m_rectangularActor);
	renderer()->RemoveActor(m_rectangularFrameActor);

	actorCollection()->RemoveItem(m_rectangularActor);
	actorCollection()->RemoveItem(m_rectangularFrameActor);

	delete m_rightClickingMenu;
}

bool GridCreatingConditionRectangularRegion::create(QWidget * /*parent*/)
{
	GridCreatingConditionRectangularRegionSettingDialog* dialog = new GridCreatingConditionRectangularRegionSettingDialog(this, preProcessorWindow());
	dialog->setXMin(m_xMin);
	dialog->setXMax(m_xMax);
	dialog->setYMin(m_yMin);
	dialog->setYMax(m_yMax);

	if (m_stepSize == 0){
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

bool GridCreatingConditionRectangularRegion::ready() const
{
	return true;
}

void GridCreatingConditionRectangularRegion::setupMenu()
{
	PreProcessorGridCreatingConditionDataItemInterface* p = dynamic_cast<PreProcessorGridCreatingConditionDataItemInterface*>(parent());

	m_rightClickingMenu = new QMenu();
	m_rightClickingMenu->addAction(p->createAction());
	m_rightClickingMenu->addSeparator();
	m_rightClickingMenu->addAction(p->clearAction());
}

void GridCreatingConditionRectangularRegion::setupActors()
{
	if (actorCollection()->GetNumberOfItems() > 0){
		vtkCollectionIterator* it = actorCollection()->NewIterator();
		it->GoToFirstItem();
		while (! it->IsDoneWithTraversal()){
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

	createRectangularRegion(m_xMin, m_xMax, m_yMin, m_yMax);
	m_rectangularActor->VisibilityOff();
	m_rectangularFrameActor->VisibilityOff();

	updateVisibilityWithoutRendering();
}

void GridCreatingConditionRectangularRegion::mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (m_mouseEventMode != meDragging){return;}
	if (event->modifiers() == Qt::ControlModifier) return;

	m_endPoint = QPoint(event->x(), event->y());
	createRectangularRegionFromMouse();
}

void GridCreatingConditionRectangularRegion::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (event->button() == Qt::LeftButton){
		if (m_mouseEventMode == meNormal){
			m_startPoint = QPoint(event->x(), event->y());
			m_mouseEventMode = meDragging;
		}
	}
	if (event->button() == Qt::RightButton){
		m_dragStartPoint = QPoint(event->x(), event->y());
	}
}

void GridCreatingConditionRectangularRegion::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton){
		if (m_mouseEventMode == meDragging){
			m_endPoint = QPoint(event->x(), event->y());
			createRectangularRegionFromMouse();
			m_mouseEventMode = meNormal;
		}
	}
	if (event->button() == Qt::RightButton){
		if (isNear(m_dragStartPoint, QPoint(event->x(), event->y()))){
			// show right-clicking menu.
			m_rightClickingMenu->move(event->globalPos());
			m_rightClickingMenu->show();
		}
	}
	updateMouseCursor(v);
}

void GridCreatingConditionRectangularRegion::createRectangularRegionFromMouse()
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

void GridCreatingConditionRectangularRegion::createRectangularRegion(double xmin, double xmax, double ymin, double ymax)
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

	if (xmin != xmax && ymin != ymax){
		actorCollection()->AddItem(m_rectangularActor);
		actorCollection()->AddItem(m_rectangularFrameActor);
	}
	updateVisibilityWithoutRendering();
}

void GridCreatingConditionRectangularRegion::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(2);
}

void GridCreatingConditionRectangularRegion::assignActionZValues(const ZDepthRange& range)
{
	m_rectangularActor->SetPosition(0, 0, range.min());
	m_rectangularFrameActor->SetPosition(0, 0, range.max());
}

void GridCreatingConditionRectangularRegion::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	m_xMin = elem.attribute("xMin").toDouble();
	m_xMax = elem.attribute("xMax").toDouble();
	m_yMin = elem.attribute("yMin").toDouble();
	m_yMax = elem.attribute("yMax").toDouble();
	m_stepSize = elem.attribute("stepSize").toDouble();
}

void GridCreatingConditionRectangularRegion::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	QString str;
	writer.writeAttribute("xMin", str.setNum(m_xMin));
	writer.writeAttribute("xMax", str.setNum(m_xMax));
	writer.writeAttribute("yMin", str.setNum(m_yMin));
	writer.writeAttribute("yMax", str.setNum(m_yMax));
	writer.writeAttribute("stepSize", str.setNum(m_stepSize));
}

bool GridCreatingConditionRectangularRegion::createGrid(double xmin, double xmax, double ymin, double ymax, double step)
{
	Structured2DGrid* grid = createGridInner(xmin, xmax, ymin, ymax, step);
	if (grid == 0){return false;}
	m_xMin = xmin;
	m_xMax = xmax;
	m_yMin = ymin;
	m_yMax = ymax;
	m_stepSize = step;
	emit gridCreated(grid);
	iRICUndoStack::instance().clear();
	return true;
}

void GridCreatingConditionRectangularRegion::previewGrid(double xmin, double xmax, double ymin, double ymax, double step)
{
	createRectangularRegion(xmin, xmax, ymin, ymax);
	Structured2DGrid* grid = createGridInner(xmin, xmax, ymin, ymax, step);
	if (grid == 0){return;}
	vtkDataSetMapper* mapper = vtkDataSetMapper::New();
	mapper->SetInputData(grid->vtkGrid());
	vtkActor* actor = vtkActor::New();
	actor->SetMapper(mapper);
	mapper->Delete();
	actor->VisibilityOn();
	actor->GetProperty()->SetColor(0, 0, 0);
	actor->GetProperty()->SetRepresentationToWireframe();
	renderer()->AddActor(actor);

	renderGraphicsView();

	actor->VisibilityOff();
	renderer()->RemoveActor(actor);
	actor->Delete();
}

Structured2DGrid* GridCreatingConditionRectangularRegion::createGridInner(double xmin, double xmax, double ymin, double ymax, double stepSize)
{
	Structured2DGrid* grid = new Structured2DGrid(0);
	PreProcessorGridTypeDataItemInterface* gt = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(m_conditionDataItem->parent()->parent());
	gt->gridType()->buildGridRelatedConditions(grid);

	unsigned int imax = floor((xmax - xmin) / stepSize);
	unsigned int jmax = floor((ymax - ymin) / stepSize);
	if (imax == 0 || jmax == 0){return 0;}

	imax += 1;
	jmax += 1;

	if (imax * jmax > MAXGRIDSIZE || imax * jmax < 0){
		QMessageBox::warning(dataModel()->mainWindow(), tr("Warning"), tr("The maximum number of grid nodes is %1.").arg(MAXGRIDSIZE));
		return 0;
	}
	grid->setDimensions(imax, jmax);
	vtkPoints* points = vtkPoints::New();
	points->SetDataTypeToDouble();
	points->Allocate((imax - 1) * (jmax - 1));
	for (unsigned int j = 0; j < jmax; j++){
		for (unsigned int i = 0; i < imax; i++){
			points->InsertPoint(imax * j + i, xmin + stepSize * i, ymin + stepSize * j, 0.);
		}
	}
	grid->vtkGrid()->SetPoints(points);

	// allocate memory for all grid related conditions.
	QList<GridRelatedConditionContainer*>& clist = grid->gridRelatedConditions();
	for (auto it = clist.begin(); it != clist.end(); ++it){
		(*it)->allocate();
	}
	grid->setModified();
	return grid;
}

void GridCreatingConditionRectangularRegion::hideTmpGrid()
{
	m_mouseEventMode = meNormal;
//	m_tmpActor->VisibilityOff();
}

void GridCreatingConditionRectangularRegion::informSelection(PreProcessorGraphicsViewInterface* v)
{
	actorCollection()->AddItem(m_rectangularActor);
	actorCollection()->AddItem(m_rectangularFrameActor);
	updateMouseCursor(v);
	updateVisibility();
}

void GridCreatingConditionRectangularRegion::informDeselection(PreProcessorGraphicsViewInterface* v)
{
	m_rectangularFrameActor->VisibilityOff();
	m_rectangularActor->VisibilityOff();
	v->unsetCursor();
	actorCollection()->RemoveAllItems();
}

void GridCreatingConditionRectangularRegion::clear()
{
	m_xMin = 0;
	m_xMax = 0;
	m_yMin = 0;
	m_yMax = 0;
	m_stepSize = 0;

	if (m_rectangularActor != 0){
		m_rectangularActor->VisibilityOff();
		m_rectangularFrameActor->VisibilityOff();
	}
}

void GridCreatingConditionRectangularRegion::updateMouseCursor(PreProcessorGraphicsViewInterface* v)
{
	v->setCursor(Qt::CrossCursor);
}

void GridCreatingConditionRectangularRegion::showInitialDialog()
{
	InformationDialog::information(preProcessorWindow(), GridCreatingConditionRectangularRegion::tr("Information"), GridCreatingConditionRectangularRegion::tr("Please define grid region by mouse-dragging."), "gcrectangular_region");
}
