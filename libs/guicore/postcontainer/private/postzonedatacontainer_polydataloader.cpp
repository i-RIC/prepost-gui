#include "../../misc/cgnsutil.h"
#include "../../project/projectcgnsfile.h"
#include "postzonedatacontainer_polydataloader.h"

#include <misc/stringtool.h>

#include <QPointF>
#include <QRegExp>

#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkPoints.h>
#include <vtkPolygon.h>

#include <vector>

#include <h5cgnspolydatasolution.h>
#include <public/h5cgnspolydatasolution_groupreader.h>
#include <iriclib.h>

namespace {

int loadPolyData(const std::string& name, vtkPolyData* polyData, std::vector<int>* ids, iRICLib::H5CgnsPolyDataSolution* sol, const QPointF& offset)
{
	std::vector<int> typeVec;
	std::vector<int> sizeVec;
	std::vector<double> coordXVec;
	std::vector<double> coordYVec;

	std::vector<int> cellCounts;
	std::vector<int> cellIntVals;
	std::vector<double> cellRealVals;

	sol->readTypes(name, &typeVec);
	sol->readSizes(name, &sizeVec);
	sol->readCoordinatesX(name, &coordXVec);
	sol->readCoordinatesY(name, &coordYVec);

	auto points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	auto lines = vtkSmartPointer<vtkCellArray>::New();
	auto polys = vtkSmartPointer<vtkCellArray>::New();

	int startIdx = 0;
	for (unsigned int i = 0; i < typeVec.size(); ++i) {
		int t = typeVec.at(i);
		int s = sizeVec.at(i);

		if (t == IRIC_POLYDATA_POLYGON) {
			auto vtkPol = vtkSmartPointer<vtkPolygon>::New();
			auto polygonPoints = vtkPol->GetPoints();
			auto polygonIds = vtkPol->GetPointIds();
			for (int j = 0; j < s; ++j) {
				points->InsertNextPoint(coordXVec.at(startIdx + j) - offset.x(), coordYVec.at(startIdx + j) - offset.y(), 0);

				polygonPoints->InsertNextPoint(coordXVec.at(startIdx + j) - offset.x(), coordYVec.at(startIdx + j) - offset.y(), 0);
				polygonIds->InsertNextId(j);
			}
			auto triIds = vtkSmartPointer<vtkIdList>::New();
			vtkPol->Triangulate(triIds);
			vtkIdType tri_ids[3];
			vtkIdType triFirst = 0;

			int cellCount = 0;
			while (triFirst < triIds->GetNumberOfIds()) {
				for (int j = 0; j < 3; ++j) {
					tri_ids[j] = triIds->GetId(triFirst + j) + startIdx;
				}
				polys->InsertNextCell(3, tri_ids);
				ids->push_back(i);
				triFirst += 3;
				++ cellCount;
			}
			cellCounts.push_back(cellCount);
		} else {
			std::vector<vtkIdType> tmp_ids;
			for (int j = 0; j < s; ++j) {
				points->InsertNextPoint(coordXVec.at(startIdx + j) - offset.x(), coordYVec.at(startIdx + j) - offset.y(), 0);
				tmp_ids.push_back(startIdx + j);
			}
			lines->InsertNextCell(tmp_ids.size(), tmp_ids.data());
			cellCounts.push_back(1);
			ids->push_back(i);
		}
		startIdx += s;
	}
	polyData->SetPoints(points);
	polyData->SetLines(lines);
	polyData->SetPolys(polys);

	// load values
	auto reader = sol->groupReader(name);
	reader.setCellCounts(cellCounts);
	int ier = CgnsUtil::loadScalarData(&reader, polyData->GetCellData());
	if (ier != 0) {return ier;}

	return 0;
}

} // namespace

bool PostZoneDataContainer::PolyDataLoader::load(std::map<std::string, vtkSmartPointer<vtkPolyData> >* polyDataMap, std::map<std::string, std::vector<int> >* polyDataCellIdsMap, iRICLib::H5CgnsPolyDataSolution* sol, const QPointF& offset)
{
	if (sol == nullptr) { return true; }

	polyDataMap->clear();

	std::vector<std::string> groupNames;
	int ier = sol->readGroupNames(&groupNames);
	if (ier != 0) {return false;}

	for (auto name : groupNames) {
		auto polyData = vtkSmartPointer<vtkPolyData>::New();
		std::vector<int> ids;
		int ret = loadPolyData(name, polyData.Get(), &ids, sol, offset);
		if (ret != 0) {return false;}

		polyDataMap->insert({name, polyData});
		polyDataCellIdsMap->insert({name, ids});
	}

	return true;
}
