#include "geodatapointmap_tinmanager_triangleswithlongedgeremover.h"

#include <algorithm>
#include <vector>

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

double GeoDataPointmap::TINManager::TrianglesWithLongEdgeRemover::thresholdLength(vtkPolyData* data, double rate)
{
	// build edgeLengthSqrts

	std::vector<double> edgeLengthSqrts;
	int numTris = data->GetNumberOfCells();
	edgeLengthSqrts.reserve(numTris * 3);

	auto points = data->GetPoints();
	for (vtkIdType i = 0; i < numTris; ++i) {
		// the cell is a triangle.
		auto cell = data->GetCell(i);
		double p[3][3];
		for (int j = 0; j < 3; ++j) {
			points->GetPoint(cell->GetPointId(j), &(p[j][0]));
		}

		for (int j = 0; j < 3; ++j) {
			double edgeLengthSqrt = calcEdgeLengthSqrt(&(p[j][0]), &(p[(j + 1) % 3][0]));
			edgeLengthSqrts.push_back(edgeLengthSqrt);
		}
	}
	std::sort(edgeLengthSqrts.begin(), edgeLengthSqrts.end());

	if (rate < 0) {rate = 0;}
	if (rate > 1) {rate = 1;}

	auto maxIndex = edgeLengthSqrts.size() - 1;
	auto index = static_cast<int> (maxIndex * rate);
	return std::sqrt(edgeLengthSqrts[index]);
}

vtkCellArray* GeoDataPointmap::TINManager::TrianglesWithLongEdgeRemover::buildCellArray(vtkPolyData* data, double limitEdge, bool rescue, bool deleted)
{
	double limitEdgeSqrt = limitEdge * limitEdge;
	auto points = data->GetPoints();

	std::vector<vtkIdType> cellsToDelete;

	double numTris = data->GetNumberOfCells();
	for (vtkIdType i = 0; i < numTris; ++i) {
		// the cell is a triangle.
		auto cell = data->GetCell(i);
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
			cellsToDelete.push_back(i);
		}
	}

	if (rescue) {
		// if all cells that use a point is deleted, add the cells with shortest edges.
		rescuePoints(&cellsToDelete, data);
	}

	auto cellArray = vtkCellArray::New();
	if (deleted) {
		for (vtkIdType cellId : cellsToDelete) {
			cellArray->InsertNextCell(data->GetCell(cellId));
		}
	} else {
		for (vtkIdType i = 0; i < numTris; ++i) {
			if (! std::binary_search(cellsToDelete.begin(), cellsToDelete.end(), i)) {
				cellArray->InsertNextCell(data->GetCell(i));
			}
		}
	}
	return cellArray;
}

void GeoDataPointmap::TINManager::TrianglesWithLongEdgeRemover::rescuePoints(std::vector<vtkIdType>* cellsToDelete, vtkPolyData* data)
{
	// Build pointCells
	std::map<vtkIdType, std::vector<vtkIdType> > pointCells;
	for (vtkIdType i = 0; i < data->GetNumberOfCells(); ++i) {
		auto cell = data->GetCell(i);
		for (int j = 0; j < cell->GetNumberOfPoints(); ++j) {
			vtkIdType pid = cell->GetPointId(j);
			auto it = pointCells.find(pid);
			if (it == pointCells.end()) {
				std::vector<vtkIdType> tmpCells;
				tmpCells.push_back(i);
				pointCells.insert({pid, tmpCells});
			} else {
				it->second.push_back(i);
			}
		}
	}

	std::set<vtkIdType> pointIds;
	for (vtkIdType cellId : *cellsToDelete) {
		vtkCell* cell = data->GetCell(cellId);
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
			if (! std::binary_search(cellsToDelete->begin(), cellsToDelete->end(), cellId)) {
				allCellsRemoved = false;
				break;
			}
		}
		if (! allCellsRemoved) {continue;}

		rescueCellWithShortestEdges(cellsForPoint, cellsToDelete, data);
	}
}

void GeoDataPointmap::TINManager::TrianglesWithLongEdgeRemover::rescueCellWithShortestEdges(const std::vector<vtkIdType>& cellsForPoint, std::vector<vtkIdType>* cellsToDelete, vtkPolyData* data)
{
	std::multimap<double, vtkIdType> edgeToCell;
	auto points = data->GetPoints();

	for (vtkIdType c : cellsForPoint) {
		double longestEdge = longestEdgeLengthSqrt(data->GetCell(c), points);
		edgeToCell.insert({longestEdge, c});
	}
	auto it = std::lower_bound(cellsToDelete->begin(), cellsToDelete->end(), edgeToCell.begin()->second);
	cellsToDelete->erase(it);
}

