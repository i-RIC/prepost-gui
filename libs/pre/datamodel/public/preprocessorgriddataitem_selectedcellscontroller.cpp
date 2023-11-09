#include "../preprocessorgridshapedataitem.h"
#include "../private/preprocessorgriddataitem_impl.h"
#include "preprocessorgriddataitem_selectedcellscontroller.h"
#include "preprocessorgriddataitem_selectednodescontroller.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guicore/grid/v4grid.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/misc/mouseboundingbox.h>
#include <misc/mathsupport.h>

PreProcessorGridDataItem::SelectedCellsController::SelectedCellsController(PreProcessorGridDataItem* item) :
	SelectedAbstractCellsController(item)
{
	auto cellsProp = m_cellsActor->GetProperty();
	cellsProp->SetLighting(false);
	cellsProp->SetColor(0, 0, 0);
	cellsProp->SetOpacity(0.5);
	cellsProp->SetRepresentationToSurface();

	auto cellsLinesProp = m_cellsLinesActor->GetProperty();
	cellsLinesProp->SetLighting(false);
	cellsLinesProp->SetLineWidth(3);
	cellsLinesProp->SetColor(0, 0, 0);
	cellsLinesProp->SetRepresentationToWireframe();

	m_item->renderer()->AddActor(m_cellsActor);
	m_item->renderer()->AddActor(m_cellsLinesActor);

	m_cellsActor->VisibilityOff();
	m_cellsLinesActor->VisibilityOff();
}

PreProcessorGridDataItem::SelectedCellsController::~SelectedCellsController()
{
	m_item->renderer()->RemoveActor(m_cellsActor);
	m_item->renderer()->RemoveActor2D(m_cellsLinesActor);
}

vtkPointSet* PreProcessorGridDataItem::SelectedCellsController::data() const
{
	return m_item->impl->m_grid->grid()->vtkData()->data();
}

void PreProcessorGridDataItem::SelectedCellsController::updateSelectedCells(MouseBoundingBox* box, bool xOr, VTKGraphicsView* /*view*/)
{
	if (m_item->impl->m_grid == nullptr) {
		// grid is not setup yet.
		clearSelection();
		return;
	}

	std::unordered_set<vtkIdType> selectedDataIdsSet;
	if (xOr) {
		for (auto cellId : m_selectedDataIds) {
			selectedDataIdsSet.insert(cellId);
		}
	}

	std::vector<vtkIdType> selectedDataIdsVector;

	if (iRIC::isNear(box->startPoint(), box->endPoint())) {
		// Select the clicked cell
		QPoint end = box->endPoint();
		box->setStartPoint(end.x(), end.y());
		box->setEndPoint(end.x(), end.y());

		double point[3];
		box->vtkGrid()->GetPoint(0, point);

		// find the cell that contains point.
		double weights[4];
		vtkIdType cellId = m_item->impl->m_grid->grid()->vtkData()->findCell(point[0], point[1], 0, 0, weights);
		if (cellId >= 0) {
			selectedDataIdsVector.push_back(cellId);
		}
	} else {
		auto selectedVertices = m_item->impl->m_selectedNodesController->getSelectedVertices(box, xOr);
		selectedDataIdsVector = m_item->getCellsFromVertices(selectedVertices);
	}
	for (auto cellId : selectedDataIdsVector) {
		auto it = selectedDataIdsSet.find(cellId);
		if (it != selectedDataIdsSet.end()) {
			selectedDataIdsSet.erase(it);
		} else {
			selectedDataIdsSet.insert(cellId);
		}
	}

	std::vector<vtkIdType> selected;
	for (auto cellId : selectedDataIdsSet) {
		selected.push_back(cellId);
	}
	std::sort(selected.begin(), selected.end());
	setSelectedDataIds(selected);

	m_item->updateActionStatus();
	m_item->m_shapeDataItem->updateActionStatus();
}
