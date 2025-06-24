#include "abstractcrosssectionwindow_impl_structuredgridbuilderforunstructurededge.h"

#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/grid/v4unstructured2dgrid.h>
#include <misc/edge.h>
#include <misc/mathsupport.h>

#include <vtkCell.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>

#include <unordered_map>

AbstractCrosssectionWindow::Impl::StructuredGridBuilderForUnstructuredEdge::StructuredGridBuilderForUnstructuredEdge(Impl* impl) :
	m_impl {impl}
{}

void AbstractCrosssectionWindow::Impl::StructuredGridBuilderForUnstructuredEdge::build()
{
	auto g2d = dynamic_cast<v4Unstructured2dGrid*> (m_impl->m_window->grid());

	const auto& nodeList = m_impl->m_unstructuredNodeList;
	vtkIdType nodeCount = nodeList.size();
	m_impl->m_gridForVis->setDimensions(nodeCount, 1);
	m_impl->m_additionalGridForVis->setDimensions(nodeCount, 1);

	// setup positions
	double distanceSum = 0;

	QPointF origin(0, 0);
	m_impl->m_gridForVis->setPoint2d(0, 0, origin);
	m_impl->m_additionalGridForVis->setPoint2d(0, 0, origin);

	for (vtkIdType i = 1; i < nodeCount; ++i) {
		auto p1 = g2d->point2d(nodeList.at(i - 1));
		auto p2 = g2d->point2d(nodeList.at(i));
		distanceSum += iRIC::distance(p1, p2);

		QPointF pos(distanceSum, 0);
		m_impl->m_gridForVis->setPoint2d(i, 0, pos);
		m_impl->m_additionalGridForVis->setPoint2d(i, 0, pos);
	}

	// setup cellMap
	std::unordered_map<Edge, vtkIdType, Edge::HashFunction> cellMap;
	auto ug = g2d->vtkData()->data();
	for (vtkIdType i = 0; i < ug->GetNumberOfCells(); ++i) {
		auto cell = ug->GetCell(i);
		// expects it is a edge
		auto id0 = cell->GetPointId(0);
		auto id1 = cell->GetPointId(1);
		Edge edge(id0, id1);
		cellMap.insert({edge, i});
	}
	std::vector<vtkIdType> cellList;
	for (int i = 0; i < nodeList.size() - 1; ++i) {
		Edge edge(nodeList.at(i), nodeList.at(i + 1));
		auto it = cellMap.find(edge);
		if (it == cellMap.end()) {
			// this will not happen
			cellList.push_back(0);
		} else {
			cellList.push_back(it->second);
		}
	}

	// setup grid attributes
	copyNodeAttributes(m_impl->m_window->grid(), m_impl->m_gridForVis, nodeList);
	copyNodeAttributes(m_impl->m_window->additionalGrid(), m_impl->m_additionalGridForVis, nodeList);

	copyCellAttributes(m_impl->m_window->grid(), m_impl->m_gridForVis, cellList);
	copyCellAttributes(m_impl->m_window->additionalGrid(), m_impl->m_additionalGridForVis, cellList);

	m_impl->m_gridForVis->updateValueRangeSet();
	m_impl->m_additionalGridForVis->updateValueRangeSet();
}

void AbstractCrosssectionWindow::Impl::StructuredGridBuilderForUnstructuredEdge::copyNodeAttributes(v4Grid* src, v4Grid* tgt, const std::vector<vtkIdType>& nodeIds)
{
	if (src == nullptr) {return;}

	copyAttributes(src->vtkData()->data()->GetPointData(), tgt->vtkData()->data()->GetPointData(), nodeIds);
}

void AbstractCrosssectionWindow::Impl::StructuredGridBuilderForUnstructuredEdge::copyCellAttributes(v4Grid* src, v4Grid* tgt, const std::vector<vtkIdType>& cellIds)
{
	if (src == nullptr) {return;}

	copyAttributes(src->vtkData()->data()->GetCellData(), tgt->vtkData()->data()->GetCellData(), cellIds);
}

void AbstractCrosssectionWindow::Impl::StructuredGridBuilderForUnstructuredEdge::copyAttributes(vtkDataSetAttributes* srcAtts, vtkDataSetAttributes* tgtAtts, const std::vector<vtkIdType>& ids)
{
	tgtAtts->CopyGlobalIdsOn();
	tgtAtts->CopyAllocate(srcAtts, ids.size());

	for (vtkIdType i = 0; i < static_cast<vtkIdType> (ids.size()); ++i) {
		tgtAtts->CopyData(srcAtts, ids.at(i), i);
	}
}
