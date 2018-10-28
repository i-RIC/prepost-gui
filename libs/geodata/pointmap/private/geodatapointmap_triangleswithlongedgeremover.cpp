#include "geodatapointmap_removetrianglescommand.h"
#include "geodatapointmap_triangleswithlongedgeremover.h"
#include "geodatapointmap_triangleswithlongedgeremover_dialog.h"

#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

#include <algorithm>
#include <cmath>
#include <map>

namespace {

double calcEdgeLengthSqrt(double* p1, double* p2)
{
	double dx = *(p2 + 0) - *(p1 + 0);
	double dy = *(p2 + 1) - *(p1 + 1);

	return dx * dx + dy * dy;
}

double longestEdgeLengthSqrt(vtkCell* cell, vtkPoints* points)
{
	double p[3][3];

	for (int i = 0; i < 3; ++i) {
		points->GetPoint(cell->GetPointId(i), &(p[i][0]));
	}

	double ret = 0;
	for (int i = 0; i < 3; ++i) {
		double edgeLengthSqrt = calcEdgeLengthSqrt(&(p[i][0]), &(p[(i + 1) % 3][0]));
		if (i == 0 || ret < edgeLengthSqrt) {
			ret = edgeLengthSqrt;
		}
	}
	return ret;
}

} // namespace

GeoDataPointmap::TrianglesWithLongEdgeRemover::TrianglesWithLongEdgeRemover(GeoDataPointmap* parent) :
	m_previewPolyData {vtkPolyData::New()},
	m_parent {parent}
{
	m_previewActor.actor()->SetPosition(0, 0, m_parent->m_zDepthRange.max());
	m_previewActor.setPolyData(m_previewPolyData);
	m_previewActor.actor()->GetProperty()->SetColor(0, 0, 0);
	m_previewActor.actor()->GetProperty()->SetOpacity(0.3);
	m_parent->renderer()->AddActor(m_previewActor.actor());

	m_parent->m_mouseEventMode = meLongEdgeRemoveDialog;

	auto dialog = new Dialog(this);
	connect(dialog, SIGNAL(destroyed()), parent, SLOT(removeTrianglesWithLongEdgeEnd()));

	buildEdgeLengthSqrts();
	double len = threshouldLength(0.9);
	dialog->setlimitLength(len);

	dialog->show();
}

GeoDataPointmap::TrianglesWithLongEdgeRemover::~TrianglesWithLongEdgeRemover()
{
	m_parent->renderer()->RemoveActor(m_previewActor.actor());
	m_previewPolyData->Delete();

	m_parent->m_mouseEventMode = meNormal;
}

void GeoDataPointmap::TrianglesWithLongEdgeRemover::buildEdgeLengthSqrts()
{
	auto polyData = m_parent->m_vtkDelaunayedPolyData;
	int numTris = polyData->GetNumberOfCells();
	m_edgeLengthSqrts.reserve(numTris * 3);
	auto points = polyData->GetPoints();

	for (vtkIdType i = 0; i < numTris; ++i) {
		// the cell is a triangle.
		auto cell = polyData->GetCell(i);
		double p[3][3];
		for (int j = 0; j < 3; ++j) {
			points->GetPoint(cell->GetPointId(j), &(p[j][0]));
		}
		for (int j = 0; j < 3; ++j) {
			double edgeLengthSqrt = calcEdgeLengthSqrt(&(p[j][0]), &(p[(j + 1) % 3][0]));
			m_edgeLengthSqrts.push_back(edgeLengthSqrt);
		}
	}
	std::sort(m_edgeLengthSqrts.begin(), m_edgeLengthSqrts.end());
}

double GeoDataPointmap::TrianglesWithLongEdgeRemover::threshouldLength(double rate)
{
	if (rate < 0) {rate = 0;}
	if (rate > 1) {rate = 1;}
	double maxindex = m_edgeLengthSqrts.size() - 1;

	int index = static_cast<int> (maxindex * rate);
	return std::sqrt(m_edgeLengthSqrts[index]);
}

void GeoDataPointmap::TrianglesWithLongEdgeRemover::updatePreviewPolyData(double limitEdge, bool rescuePoints)
{
	m_previewPolyData->Initialize();
	m_previewPolyData->SetPoints(m_parent->m_vtkDelaunayedPolyData->GetPoints());

	auto cellArray = buildPolyCellArray(limitEdge, rescuePoints, true);
	m_previewPolyData->SetPolys(cellArray);
	cellArray->Delete();

	m_parent->renderGraphicsView();
}

void GeoDataPointmap::TrianglesWithLongEdgeRemover::removeTriangles(double limitEdge, bool rescuePoints)
{
	vtkCellArray* newCellArray = buildPolyCellArray(limitEdge, rescuePoints, false);
	m_parent->pushRenderCommand(new RemoveTrianglesCommand(newCellArray, m_parent));
	newCellArray->Delete();
}

vtkCellArray* GeoDataPointmap::TrianglesWithLongEdgeRemover::buildPolyCellArray(double limitEdge, bool rescuePoints, bool forPreview)
{
	double limitEdgeSqrt = limitEdge * limitEdge;
	auto polyData = m_parent->m_vtkDelaunayedPolyData;
	auto points = polyData->GetPoints();

	std::vector<vtkIdType> cells;

	double numTris = polyData->GetNumberOfCells();
	for (vtkIdType i = 0; i < numTris; ++i) {
		// the cell is a triangle.
		auto cell = polyData->GetCell(i);
		double p[3][3];
		for (int j = 0; j < 3; ++j) {
			points->GetPoint(cell->GetPointId(j), &(p[j][0]));
		}

		bool hasLongEdge = false;
		for (int j = 0; j < 3; ++j) {
			double edgeLengthSqrt = calcEdgeLengthSqrt(&(p[j][0]), &(p[(j + 1) % 3][0]));
			hasLongEdge = hasLongEdge || (edgeLengthSqrt > limitEdgeSqrt);
		}
		if (hasLongEdge) {
			cells.push_back(i);
		}
	}

	if (rescuePoints) {
		// if all cells that use a point is deleted, add the cells with shortest edges.
		this->rescuePoints(&cells);
	}

	vtkCellArray* cellArray = vtkCellArray::New();
	if (forPreview) {
		for (vtkIdType cellId : cells) {
			cellArray->InsertNextCell(polyData->GetCell(cellId));
		}
	} else {
		for (vtkIdType i = 0; i < numTris; ++i) {
			if (! std::binary_search(cells.begin(), cells.end(), i)) {
				cellArray->InsertNextCell(polyData->GetCell(i));
			}
		}
	}

	return cellArray;
}

void GeoDataPointmap::TrianglesWithLongEdgeRemover::rescuePoints(std::vector<vtkIdType>* cells)
{
	// Build pointCells
	std::map<vtkIdType, std::vector<vtkIdType> > pointCells;
	auto polyData = m_parent->m_vtkDelaunayedPolyData;
	for (vtkIdType i = 0; i < polyData->GetNumberOfCells(); ++i) {
		auto cell = polyData->GetCell(i);
		for (int j = 0; j < cell->GetNumberOfPoints(); ++j) {
			vtkIdType pid = cell->GetPointId(j);
			auto it = pointCells.find(pid);
			if (it == pointCells.end()) {
				std::vector<vtkIdType> cells;
				cells.push_back(i);
				pointCells.insert({pid, cells});
			} else {
				it->second.push_back(i);
			}
		}
	}

	std::set<vtkIdType> pointIds;
	for (vtkIdType cellId : *cells) {
		vtkCell* cell = polyData->GetCell(cellId);
		for (int j = 0; j < cell->GetNumberOfPoints(); ++j){
			pointIds.insert(cell->GetPointId(j));
		}
	}

	for (vtkIdType p : pointIds) {
		auto it = pointCells.find(p);
		if (it == pointCells.end()) {
			// this will not happen, but if it does, ignore the point.
			continue;
		}

		auto cellsForPoint = it->second;
		bool allCellsRemoved = true;
		for (vtkIdType cellId : cellsForPoint) {
			if (! std::binary_search(cells->begin(), cells->end(), cellId)) {
				allCellsRemoved = false;
				break;
			}
		}
		if (! allCellsRemoved) {continue;}

		rescueCellWithShortestEdges(cellsForPoint, cells);
	}
}

void GeoDataPointmap::TrianglesWithLongEdgeRemover::rescueCellWithShortestEdges(const std::vector<vtkIdType>& cellsForPoint, std::vector<vtkIdType>* cells)
{
	std::multimap<double, vtkIdType> edgeToCell;
	auto polyData = m_parent->m_vtkDelaunayedPolyData;
	auto points = polyData->GetPoints();
	for (vtkIdType c : cellsForPoint) {
		double shortestEdge = longestEdgeLengthSqrt(polyData->GetCell(c), points);
		edgeToCell.insert({shortestEdge, c});
	}
	auto it = std::lower_bound(cells->begin(), cells->end(), edgeToCell.begin()->second);
	cells->erase(it);
}
