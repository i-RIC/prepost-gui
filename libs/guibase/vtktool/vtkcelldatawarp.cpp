#include "vtkcelldatawarp.h"

#include <vtkQuad.h>
#include <vtkUnstructuredGrid.h>
#include <vtkPointSet.h>

#include <map>
#include <unordered_map>

namespace {

class Edge {
public:
	Edge(vtkIdType p1, vtkIdType p2) :
		point1 {p1},
		point2 {p2}
	{}

	bool operator<(const Edge& edge) const {
		if (point1 != edge.point1) {
			return point1 < edge.point1;
		} else {
			return point2 < edge.point2;
		}
	}

	vtkIdType point1;
	vtkIdType point2;
};

template <typename V>
void copyDataArray(const std::unordered_map<vtkIdType, vtkIdType>& pointMap, V* src, V* tgt) {
	for (vtkIdType tgtId = 0; tgtId < static_cast<vtkIdType>(pointMap.size()); ++tgtId) {
		vtkIdType srcId = pointMap.at(tgtId);
		tgt->InsertNextValue(src->GetValue(srcId));
	}
}

} // namespace

vtkUnstructuredGrid* vtkCellDataWarp::warp(vtkPointSet* data) {
	auto ret = vtkUnstructuredGrid::New();
	auto points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	ret->SetPoints(points);
	auto pointData = ret->GetPointData();

	auto origPoints = data->GetPoints();
	auto origPointData = data->GetPointData();
	auto origCellData = data->GetCellData();

	auto activeData = data->GetCellData()->GetScalars();
	std::unordered_map<vtkIdType, vtkIdType> reversePointMap;
	std::unordered_map<vtkIdType, vtkIdType> reversePointCellMap;
	std::map<Edge, std::vector<Edge> > edgeMap;

	vtkIdType numCells = data->GetNumberOfCells();
	for (vtkIdType cellId = 0; cellId < numCells; ++cellId) {
		std::unordered_map<vtkIdType, vtkIdType> pointMap;
		auto cell = data->GetCell(cellId);
		auto idlist = vtkSmartPointer<vtkIdList>::New();
		for (int i = 0; i < cell->GetNumberOfPoints(); ++i) {
			double p[3];
			double value;
			vtkIdType pointId = cell->GetPointId(i);
			origPoints->GetPoint(pointId, p);
			value = activeData->GetTuple1(cellId);
			p[2] = value;
			points->InsertNextPoint(p);
			auto newPointId = points->GetNumberOfPoints() - 1;
			pointMap.insert({pointId, newPointId});
			reversePointMap.insert({newPointId, pointId});
			reversePointCellMap.insert({newPointId, cellId});
			idlist->InsertNextId(newPointId);
		}
		ret->InsertNextCell(cell->GetCellType(), idlist);

		int numEdges = cell->GetNumberOfEdges();
		for (vtkIdType edgeId = 0; edgeId < numEdges; ++edgeId) {
			auto edge = cell->GetEdge(edgeId);
			auto p1 = edge->GetPointId(0);
			auto p2 = edge->GetPointId(1);
			if (p1 > p2) {
				std::swap(p1, p2);
			}
			Edge origEdge(p1, p2);
			Edge newEdge(pointMap.at(p1), pointMap.at(p2));
			auto it = edgeMap.find(origEdge);
			if (it == edgeMap.end()) {
				std::vector<Edge> emptyVec;
				auto pair = edgeMap.insert({origEdge, emptyVec});
				it = pair.first;
			}
			it->second.push_back(newEdge);
		}
	}
	for (const auto& pair : edgeMap) {
		if (pair.second.size() != 2) {continue;}
		auto e1 = pair.second.at(0);
		auto e2 = pair.second.at(1);
		vtkIdType ids[4];
		ids[0] = e1.point1;
		ids[1] = e1.point2;
		ids[2] = e2.point2;
		ids[3] = e2.point1;
		ret->InsertNextCell(VTK_QUAD, 4, ids);
	}
	for (int aId = 0; aId < origPointData->GetNumberOfArrays(); ++aId) {
		auto origArray = origPointData->GetArray(aId);
		if (origArray->GetNumberOfComponents() > 1) {continue;}
		auto origIntArray = vtkIntArray::SafeDownCast(origArray);
		auto origDblArray = vtkDoubleArray::SafeDownCast(origArray);
		if (origIntArray != nullptr) {
			auto newIntArray = vtkSmartPointer<vtkIntArray>::New();
			newIntArray->SetName(origIntArray->GetName());
			copyDataArray(reversePointMap, origIntArray, newIntArray.Get());
			pointData->AddArray(newIntArray);
		} else if (origDblArray != nullptr) {
			auto newDblArray = vtkSmartPointer<vtkDoubleArray>::New();
			newDblArray->SetName(origDblArray->GetName());
			copyDataArray(reversePointMap, origDblArray, newDblArray.Get());
			pointData->AddArray(newDblArray);
		}
	}
	for (int aId = 0; aId < origCellData->GetNumberOfArrays(); ++aId) {
		auto origArray = origCellData->GetArray(aId);
		if (origArray->GetNumberOfComponents() > 1) {continue;}
		auto origIntArray = vtkIntArray::SafeDownCast(origArray);
		auto origDblArray = vtkDoubleArray::SafeDownCast(origArray);
		if (origIntArray != nullptr) {
			auto newIntArray = vtkSmartPointer<vtkIntArray>::New();
			newIntArray->SetName(origIntArray->GetName());
			copyDataArray(reversePointCellMap, origIntArray, newIntArray.Get());
			pointData->AddArray(newIntArray);
		} else if (origDblArray != nullptr) {
			auto newDblArray = vtkSmartPointer<vtkDoubleArray>::New();
			newDblArray->SetName(origDblArray->GetName());
			copyDataArray(reversePointCellMap, origDblArray, newDblArray.Get());
			pointData->AddArray(newDblArray);
		}
	}
	return ret;
}

vtkCellDataWarp::vtkCellDataWarp()
{}
