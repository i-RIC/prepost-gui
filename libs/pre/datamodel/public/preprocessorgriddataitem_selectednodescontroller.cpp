#include "../preprocessorgridshapedataitem.h"
#include "../private/preprocessorgriddataitem_impl.h"
#include "preprocessorgriddataitem_selectednodescontroller.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guicore/datamodel/vtk2dgraphicsview.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/misc/mouseboundingbox.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <misc/mathsupport.h>

#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

PreProcessorGridDataItem::SelectedNodesController::SelectedNodesController(PreProcessorGridDataItem* item) :
	SelectedDataWithIdController(item),
	m_selectedData {vtkPolyData::New()},
	m_mapper {vtkPolyDataMapper::New()},
	m_actor {vtkActor::New()}
{
	auto prop = m_actor->GetProperty();
	prop->SetPointSize(5);
	prop->SetLighting(false);
	prop->SetColor(0, 0, 0);
	prop->SetRepresentationToPoints();

	m_mapper->ScalarVisibilityOff();

	m_mapper->SetInputData(m_selectedData);
	m_actor->SetMapper(m_mapper);

	m_item->renderer()->AddActor(m_actor);
	m_actor->VisibilityOff();
}

PreProcessorGridDataItem::SelectedNodesController::~SelectedNodesController()
{
	m_item->renderer()->RemoveActor(m_actor);

	m_selectedData->Delete();
	m_mapper->Delete();
	m_actor->Delete();
}

void PreProcessorGridDataItem::SelectedNodesController::handleDataUpdate()
{
	if (m_item->impl->m_grid != nullptr) {
		m_selectedData->Initialize();
	} else {
		m_selectedData->SetPoints(m_item->impl->m_grid->grid()->vtkData()->data()->GetPoints());
	}
}

void PreProcessorGridDataItem::SelectedNodesController::setVisibility(bool visible)
{
	m_actor->VisibilityOff();
	m_item->m_actorCollection->RemoveItem(m_actor);
	if (visible) {
		m_item->m_actorCollection->AddItem(m_actor);
	}
	m_item->updateVisibilityWithoutRendering();
}

void PreProcessorGridDataItem::SelectedNodesController::handleMouseMoveEvent(QMouseEvent* event, VTKGraphicsView* /*view*/)
{
	auto box = m_item->dataModel()->mouseBoundingBox();
	box->setEndPoint(event->x(), event->y());

	m_item->renderGraphicsView();
}

void PreProcessorGridDataItem::SelectedNodesController::handleMousePressEvent(QMouseEvent* event, VTKGraphicsView* view)
{
	auto box = m_item->dataModel()->mouseBoundingBox();
	box->setStartPoint(event->x(), event->y());
	box->enable();

	view->GetRenderWindow()->SetDesiredUpdateRate(PreProcessorDataItem::dragUpdateRate);
	m_item->renderGraphicsView();
}

void PreProcessorGridDataItem::SelectedNodesController::handleMouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* view)
{
	auto box = m_item->dataModel()->mouseBoundingBox();
	box->setEndPoint(event->x(), event->y());

	bool xOr = ((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier);
	if (iRIC::isNear(box->startPoint(), box->endPoint())) {
		double maxDistance = view->stdDistance(iRIC::nearRadius());
		auto view2 = dynamic_cast<VTK2DGraphicsView*> (view);
		QPointF pos = view2->viewportToWorld(event->pos());
		updateSelectedVerticesBySelectingNearest(pos, maxDistance, xOr);
	} else {
		updateSelectedVertices(box, xOr);
	}
	box->disable();

	view->restoreUpdateRate();
	m_item->renderGraphicsView();
}

vtkActor* PreProcessorGridDataItem::SelectedNodesController::actor() const
{
	return m_actor;
}

std::unordered_set<vtkIdType> PreProcessorGridDataItem::SelectedNodesController::getSelectedVertices(MouseBoundingBox* box, bool xOr)
{
	std::unordered_set<vtkIdType> ret;

	if (m_item->impl->m_grid == nullptr) {
		return ret;
	}

	vtkPoints* points = m_item->impl->m_grid->grid()->vtkData()->data()->GetPoints();
	double p[3];
	auto sgrid = dynamic_cast<v4Structured2dGrid*>(m_item->impl->m_grid->grid());
	if (sgrid != nullptr) {
		for (int ii = sgrid->drawnIMin(); ii <= sgrid->drawnIMax(); ++ii) {
			for (int jj = sgrid->drawnJMin(); jj <= sgrid->drawnJMax(); ++jj) {
				vtkIdType idx = sgrid->pointIndex(ii, jj);
				points->GetPoint(idx, p);
				if (box->isInsideBox(p[0], p[1])) {
					ret.insert(idx);
				}
			}
		}
	} else {
		for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
			points->GetPoint(i, p);
			if (box->isInsideBox(p[0], p[1])) {
				ret.insert(i);
			}
		}
	}
	return ret;
}

void PreProcessorGridDataItem::SelectedNodesController::updateSelectedVerticesBySelectingNearest(QPointF& pos, double maxDistance, bool xOr)
{
	if (m_item->impl->m_grid == nullptr) {
		// grid is not setup yet.
		clearSelection();
		return;
	}

	std::unordered_set<vtkIdType> selectedDataIdsSet;
	if (xOr) {
		for (auto id : m_selectedDataIds) {
			selectedDataIdsSet.insert(id);
		}
	}
	vtkPoints* points = m_item->impl->m_grid->grid()->vtkData()->data()->GetPoints();

	double p[3];
	auto sgrid = dynamic_cast<v4Structured2dGrid*>(m_item->impl->m_grid->grid());
	std::map<double, vtkIdType> distanceMap;

	if (sgrid != nullptr) {
		for (int ii = sgrid->drawnIMin(); ii <= sgrid->drawnIMax(); ++ii) {
			for (int jj = sgrid->drawnJMin(); jj <= sgrid->drawnJMax(); ++jj) {
				vtkIdType idx = sgrid->pointIndex(ii, jj);
				points->GetPoint(idx, p);
				QPointF pos2(p[0], p[1]);
				double distance = iRIC::distance(pos, pos2);
				if (distance > maxDistance) {continue;}

				distanceMap.insert({distance, idx});
			}
		}
	} else {
		for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
			points->GetPoint(i, p);
			QPointF pos2(p[0], p[1]);
			double distance = iRIC::distance(pos, pos2);
			if (distance > maxDistance) {continue;}

			distanceMap.insert({distance, i});
		}
	}

	if (distanceMap.size() > 0) {
		auto nearestId = distanceMap.begin()->second;
		auto it = selectedDataIdsSet.find(nearestId);
		if (it != selectedDataIdsSet.end()) {
			selectedDataIdsSet.erase(it);
		} else {
			selectedDataIdsSet.insert(nearestId);
		}
	}
	std::vector<vtkIdType> selected;
	selected.reserve(selectedDataIdsSet.size());
	for (auto vid : selectedDataIdsSet) {
		selected.push_back(vid);
	}
	std::sort(selected.begin(), selected.end());

	setSelectedDataIds(selected);
	m_item->updateActionStatus();
	m_item->m_shapeDataItem->updateActionStatus();
}

void PreProcessorGridDataItem::SelectedNodesController::updateSelectedVertices(MouseBoundingBox* box, bool xOr)
{
	if (m_item->impl->m_grid == nullptr) {
		// grid is not setup yet.
		clearSelection();
		return;
	}

	std::unordered_set<vtkIdType> selectedDataIdsSet;
	if (xOr) {
		for (auto id : m_selectedDataIds) {
			selectedDataIdsSet.insert(id);
		}
	}

	auto newSelectedDataIdSet = getSelectedVertices(box, xOr);
	for (auto cellId : newSelectedDataIdSet) {
		auto it = selectedDataIdsSet.find(cellId);
		if (it != selectedDataIdsSet.end()) {
			selectedDataIdsSet.erase(it);
		} else {
			selectedDataIdsSet.insert(cellId);
		}
	}

	std::vector<vtkIdType> selected;
	selected.reserve(selectedDataIdsSet.size());
	for (auto vid : selectedDataIdsSet) {
		selected.push_back(vid);
	}
	std::sort(selected.begin(), selected.end());

	setSelectedDataIds(selected);
	m_item->updateActionStatus();
	m_item->m_shapeDataItem->updateActionStatus();
}

void PreProcessorGridDataItem::SelectedNodesController::updateVtkObjects()
{
	m_selectedData->Reset();

	if (m_item->impl->m_grid == nullptr) {return;}

	m_selectedData->SetPoints(m_item->impl->m_grid->grid()->vtkData()->data()->GetPoints());
	auto cells = vtkSmartPointer<vtkCellArray>::New();
	for (vtkIdType pointId : m_selectedDataIds) {
		cells->InsertNextCell(1, &pointId);
	}
	m_selectedData->SetVerts(cells);
	m_selectedData->Modified();
}
