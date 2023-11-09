#include "../preprocessorgridshapedataitem.h"
#include "../private/preprocessorgriddataitem_impl.h"
#include "preprocessorgriddataitem_selectededgescontroller.h"
#include "preprocessorgriddataitem_selectednodescontroller.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guicore/datamodel/vtk2dgraphicsview.h>
#include <guicore/grid/v4grid2d.h>
#include <guicore/misc/mouseboundingbox.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <misc/mathsupport.h>

PreProcessorGridDataItem::SelectedEdgesController::SelectedEdgesController(PreProcessorGridDataItem* item) :
	SelectedDataController {item},
	m_selectedData {vtkPolyData::New()},
	m_mapper {vtkPolyDataMapper::New()},
	m_actor {vtkActor::New()}
{
	auto prop = m_actor->GetProperty();
	prop->SetLighting(false);
	prop->SetLineWidth(3);
	prop->SetColor(0, 0, 0);

	m_mapper->ScalarVisibilityOff();

	m_mapper->SetInputData(m_selectedData);
	m_actor->SetMapper(m_mapper);

	m_item->renderer()->AddActor(m_actor);
	m_actor->VisibilityOff();
}

PreProcessorGridDataItem::SelectedEdgesController::~SelectedEdgesController()
{
	m_item->renderer()->RemoveActor(m_actor);

	m_selectedData->Delete();
	m_mapper->Delete();
	m_actor->Delete();
}

const std::vector<Edge>& PreProcessorGridDataItem::SelectedEdgesController::selectedEdges() const
{
	return m_selectedEdges;
}

void PreProcessorGridDataItem::SelectedEdgesController::setSelectedEdges(const std::vector<Edge>& edges)
{
	m_selectedEdges = edges;
	updateVtkObjects();
}

void PreProcessorGridDataItem::SelectedEdgesController::handleDataUpdate()
{
	if (m_item->impl->m_grid != nullptr) {
		m_selectedData->Initialize();
	} else {
		m_selectedData->SetPoints(m_item->impl->m_grid->grid()->vtkData()->data()->GetPoints());
	}
}

void PreProcessorGridDataItem::SelectedEdgesController::setVisibility(bool visible)
{
	m_actor->VisibilityOff();
	m_item->m_actorCollection->RemoveItem(m_actor);
	if (visible) {
		m_item->m_actorCollection->AddItem(m_actor);
	}
	m_item->updateVisibilityWithoutRendering();
}

void PreProcessorGridDataItem::SelectedEdgesController::handleMouseMoveEvent(QMouseEvent* event, VTKGraphicsView* /*view*/)
{
	auto box = m_item->dataModel()->mouseBoundingBox();
	box->setEndPoint(event->x(), event->y());

	m_item->renderGraphicsView();
}

void PreProcessorGridDataItem::SelectedEdgesController::handleMousePressEvent(QMouseEvent* event, VTKGraphicsView* view)
{
	auto box = m_item->dataModel()->mouseBoundingBox();
	box->setStartPoint(event->x(), event->y());
	box->enable();

	view->GetRenderWindow()->SetDesiredUpdateRate(PreProcessorDataItem::dragUpdateRate);
	m_item->renderGraphicsView();
}

void PreProcessorGridDataItem::SelectedEdgesController::handleMouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* view)
{
	auto box = m_item->dataModel()->mouseBoundingBox();
	box->setEndPoint(event->x(), event->y());

	bool xOr = ((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier);
	updateSelectedEdges(box, xOr, view);
	box->disable();

	view->restoreUpdateRate();
	m_item->renderGraphicsView();
}

void PreProcessorGridDataItem::SelectedEdgesController::clearSelection()
{
	std::vector<Edge> empty;
	setSelectedEdges(empty);
}

vtkActor* PreProcessorGridDataItem::SelectedEdgesController::actor()
{
	return m_actor;
}

void PreProcessorGridDataItem::SelectedEdgesController::updateSelectedEdges(MouseBoundingBox* box, bool xOr, VTKGraphicsView* view)
{
	if (m_item->impl->m_grid == nullptr) {
		// grid is not setup yet.
		clearSelection();
		return;
	}
	bool click = false;
	if (iRIC::isNear(box->startPoint(), box->endPoint())) {
		QPoint center;
		center.setX((box->startPoint().x() + box->endPoint().x()) / 2);
		center.setY((box->startPoint().y() + box->endPoint().y()) / 2);
		box->setStartPoint(center.x(), center.y());
		box->setEndPoint(center.x(), center.y());
		click = true;
	}

	std::unordered_set<Edge, Edge::HashFunction> selectedEdgesSet;
	if (xOr) {
		for (const auto& edge : m_selectedEdges) {
			selectedEdgesSet.insert(edge);
		}
	}

	std::vector<Edge> selectedEdgesVector;
	if (click) {
		double p[3];
		auto v2 = dynamic_cast<VTK2DGraphicsView*>(view);
		double stdDist = v2->stdRadius(iRIC::nearRadius());
		auto cellIds = vtkSmartPointer<vtkIdList>::New();
		box->vtkGrid()->GetPoint(0, p);
		vtkPolyData* pd = m_item->buildEdges();
		vtkIdType pointId = pd->FindPoint(p);
		pd->GetPointCells(pointId, cellIds);
		for (vtkIdType i = 0; i < cellIds->GetNumberOfIds(); ++i) {
			vtkCell* cell = pd->GetCell(cellIds->GetId(i));
			Edge edge(cell->GetPointId(0), cell->GetPointId(1));

			// check whether cell is clicked.
			double tmpp[3];
			pd->GetPoint(cell->GetPointId(0), tmpp);
			QPointF v0(tmpp[0], tmpp[1]);
			pd->GetPoint(cell->GetPointId(1), tmpp);
			QPointF v1(tmpp[0], tmpp[1]);

			QPointF horizontal = v1 - v0;
			QPointF vertical = iRIC::normalize(v1 - v0) * stdDist;
			vertical = iRIC::rotateVector90(vertical);
			QPointF posv = v0 - vertical * 0.5;
			QPointF point(p[0], p[1]);
			if (iRIC::isInsideParallelogram(point, posv, horizontal, vertical)) {
				selectedEdgesVector.push_back(edge);
				break;
			}
		}
		pd->Delete();
	} else {
		// setup selectedVertices.
		auto selectedVertices = m_item->impl->m_selectedNodesController->getSelectedVertices(box, false);
		selectedEdgesVector = m_item->getEdgesFromVertices(selectedVertices);
	}
	for (const auto& e : selectedEdgesVector) {
		auto it = selectedEdgesSet.find(e);
		if (it != selectedEdgesSet.end()) {
			selectedEdgesSet.erase(it);
		} else {
			selectedEdgesSet.insert(e);
		}
	}

	std::vector<Edge> selected;
	for (const auto& e : selectedEdgesSet) {
		selected.push_back(e);
	}
	std::sort(selected.begin(), selected.end());
	setSelectedEdges(selected);

	m_item->updateActionStatus();
	m_item->m_shapeDataItem->updateActionStatus();
}

void PreProcessorGridDataItem::SelectedEdgesController::updateVtkObjects()
{
	m_selectedData->Reset();

	if (m_item->impl->m_grid == nullptr) {return;}

	m_selectedData->SetPoints(m_item->impl->m_grid->grid()->vtkData()->data()->GetPoints());
	auto cells = vtkSmartPointer<vtkCellArray>::New();
	vtkIdType nodes[2];
	for (const auto& edge : m_selectedEdges) {
		nodes[0] = edge.vertex1();
		nodes[1] = edge.vertex2();
		cells->InsertNextCell(2, &nodes[0]);
	}
	m_selectedData->SetLines(cells);
	m_selectedData->Modified();
}
