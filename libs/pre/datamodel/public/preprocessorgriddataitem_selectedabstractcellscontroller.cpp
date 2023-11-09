#include "preprocessorgriddataitem_selectedabstractcellscontroller.h"
#include "../private/preprocessorgriddataitem_impl.h"

#include <guicore/datamodel/vtk2dgraphicsview.h>
#include <guicore/misc/mouseboundingbox.h>

#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkExtractCells.h>
#include <vtkProperty.h>

PreProcessorGridDataItem::SelectedAbstractCellsController::SelectedAbstractCellsController(PreProcessorGridDataItem* item) :
	SelectedDataWithIdController {item},
	m_emptyData {vtkPolyData::New()},
	m_extractCells {vtkExtractCells::New()},
	m_cellsMapper {vtkDataSetMapper::New()},
	m_cellsActor {vtkActor::New()},
	m_cellsLinesMapper {vtkDataSetMapper::New()},
	m_cellsLinesActor {vtkActor::New()}
{
	m_cellsMapper->SetScalarVisibility(false);

	m_cellsLinesMapper->SetScalarVisibility(false);

	m_cellsMapper->SetInputConnection(m_extractCells->GetOutputPort());
	m_cellsActor->SetMapper(m_cellsMapper);

	m_cellsLinesMapper->SetInputConnection(m_extractCells->GetOutputPort());
	m_cellsLinesActor->SetMapper(m_cellsLinesMapper);
}

PreProcessorGridDataItem::SelectedAbstractCellsController::~SelectedAbstractCellsController()
{
	m_emptyData->Delete();
	m_extractCells->Delete();
	m_cellsMapper->Delete();
	m_cellsActor->Delete();
	m_cellsLinesMapper->Delete();
	m_cellsLinesActor->Delete();
}

void PreProcessorGridDataItem::SelectedAbstractCellsController::handleDataUpdate()
{
	if (m_item->impl->m_grid != nullptr) {
		m_extractCells->SetInputData(data());
	} else {
		m_extractCells->SetInputData(m_emptyData);
	}
}

void PreProcessorGridDataItem::SelectedAbstractCellsController::setVisibility(bool visible)
{
	m_cellsActor->VisibilityOff();
	m_cellsLinesActor->VisibilityOff();

	auto col = m_item->m_actorCollection;
	col->RemoveItem(m_cellsActor);
	col->RemoveItem(m_cellsLinesActor);
	if (visible) {
		col->AddItem(m_cellsActor);
		col->AddItem(m_cellsLinesActor);
	}
	m_item->updateVisibilityWithoutRendering();
}

void PreProcessorGridDataItem::SelectedAbstractCellsController::handleMouseMoveEvent(QMouseEvent* event, VTKGraphicsView* /*view*/)
{
	auto box = m_item->dataModel()->mouseBoundingBox();
	box->setEndPoint(event->x(), event->y());

	m_item->renderGraphicsView();
}

void PreProcessorGridDataItem::SelectedAbstractCellsController::handleMousePressEvent(QMouseEvent* event, VTKGraphicsView* view)
{
	auto box = m_item->dataModel()->mouseBoundingBox();
	box->setStartPoint(event->x(), event->y());
	box->enable();

	view->GetRenderWindow()->SetDesiredUpdateRate(PreProcessorDataItem::dragUpdateRate);
	m_item->renderGraphicsView();
}

void PreProcessorGridDataItem::SelectedAbstractCellsController::handleMouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* view)
{
	auto box = m_item->dataModel()->mouseBoundingBox();
	box->setEndPoint(event->x(), event->y());

	bool xOr = ((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier);
	updateSelectedCells(box, xOr, view);
	box->disable();

	view->restoreUpdateRate();
	m_item->renderGraphicsView();
}

vtkActor* PreProcessorGridDataItem::SelectedAbstractCellsController::cellsActor() const
{
	return m_cellsActor;
}

vtkActor* PreProcessorGridDataItem::SelectedAbstractCellsController::cellsLinesActor()
{
	return m_cellsLinesActor;
}

void PreProcessorGridDataItem::SelectedAbstractCellsController::updateVtkObjects()
{
	auto cellIds = vtkSmartPointer<vtkIdList>::New();
	cellIds->Allocate(m_selectedDataIds.size());
	for (auto cellId : m_selectedDataIds) {
		cellIds->InsertNextId(cellId);
	}
	m_extractCells->SetCellList(cellIds);
	m_extractCells->Modified();
}
