#include "../private/preprocessorgriddataitem_impl.h"
#include "preprocessorgriddataitem_selectedjedgescontroller.h"
#include "preprocessorgriddataitem_selectednodescontroller.h"

#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>
#include <guicore/datamodel/vtk2dgraphicsview.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/misc/mouseboundingbox.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <misc/mathsupport.h>

PreProcessorGridDataItem::SelectedJEdgesController::SelectedJEdgesController(PreProcessorGridDataItem* item) :
	SelectedAbstractCellsController {item}
{
	auto cellsLinesProp = m_cellsLinesActor->GetProperty();
	cellsLinesProp->SetLighting(false);
	cellsLinesProp->SetLineWidth(5);
	cellsLinesProp->SetOpacity(0.5);
	cellsLinesProp->SetColor(0, 0, 0);
	cellsLinesProp->SetRepresentationToWireframe();

	m_item->renderer()->AddActor(m_cellsLinesActor);

	m_cellsLinesActor->VisibilityOff();
}

PreProcessorGridDataItem::SelectedJEdgesController::~SelectedJEdgesController()
{
	m_item->renderer()->RemoveActor(m_cellsLinesActor);
}

vtkPointSet* PreProcessorGridDataItem::SelectedJEdgesController::data() const
{
	auto grid = dynamic_cast<v4Structured2dGrid*> (m_item->impl->m_grid->grid());
	if (grid == nullptr) {return nullptr;}

	return grid->vtkJEdgeData()->data();
}

void PreProcessorGridDataItem::SelectedJEdgesController::updateSelectedCells(MouseBoundingBox* box, bool xOr, VTKGraphicsView* view)
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
		double weights[2];

		auto sGrid = dynamic_cast<v4Structured2dGrid*> (m_item->impl->m_grid->grid());
		auto jEdgeGrid = sGrid->vtkJEdgeData();

		auto view2 = dynamic_cast<VTK2DGraphicsView*> (view);
		vtkIdType cellId = jEdgeGrid->findCell(point[0], point[1], 0, view2->stdRadius(iRIC::nearRadius()), weights);
		if (cellId >= 0) {
			selectedDataIdsVector.push_back(cellId);
		}
	} else {
		auto selectedVertices = m_item->impl->m_selectedNodesController->getSelectedVertices(box, xOr);
		selectedDataIdsVector = m_item->getJEdgesFromVertices(selectedVertices);
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
}
