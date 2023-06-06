#include "../../misc/cgnsutil.h"
#include "../../project/projectcgnsfile.h"
#include "postzonedatacontainer_polydataloader.h"

#include <geoio/polygonutil.h>
#include <misc/stringtool.h>

#include <QPointF>
#include <QPolygonF>
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

	sol->readTypes(name, &typeVec);
	sol->readSizes(name, &sizeVec);
	sol->readCoordinatesX(name, &coordXVec);
	sol->readCoordinatesY(name, &coordYVec);

	cellCounts.reserve(typeVec.size());
	ids->reserve(typeVec.size() * 2);

	auto points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	points->Allocate(coordXVec.size());

	auto lines = vtkSmartPointer<vtkCellArray>::New();
	lines->Allocate(typeVec.size());
	auto polys = vtkSmartPointer<vtkCellArray>::New();
	polys->Allocate(typeVec.size());

	int startIdx = 0;
	vtkIdType triIds[3];
	auto vtkPol = vtkSmartPointer<vtkPolygon>::New();
	auto idList = vtkSmartPointer<vtkIdList>::New();
	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < typeVec.size(); ++i) {
		int t = typeVec.at(i);
		int s = sizeVec.at(i);

		if (t == IRIC_POLYDATA_POLYGON) {
			QPolygonF pol;
			pol.reserve(s + 1);
			for (int j = 0; j < s; ++j) {
				QPointF point(coordXVec.at(startIdx + j) - offset.x(), coordYVec.at(startIdx + j) - offset.y());
				pol.push_back(point);
				points->InsertNextPoint(point.x(), point.y(), 0);
			}

			auto firstPoint = pol.first();
			auto lastPoint = pol.last();
			if (firstPoint != lastPoint) {
				pol.push_back(firstPoint);
			}

			bool allSame = true;
			for (int i = 1; i < pol.size(); ++i) {
				allSame = allSame && (pol.at(0) == pol.at(i));
			}

			indices.resize(0);
			if (! allSame) {
				PolygonUtil::triangulateVtk(pol, &indices, vtkPol, idList);
			}

			unsigned int pos = 0;
			int cellCount = 0;
			while (pos < indices.size()) {
				for (int j = 0; j < 3; ++j) {
					triIds[j] = indices.at(pos + j) + startIdx;
				}
				polys->InsertNextCell(3, triIds);
				ids->push_back(i);
				++ cellCount;
				pos += 3;
			}
			cellCounts.push_back(cellCount);
		} else {
			std::vector<vtkIdType> tmp_ids;
			tmp_ids.reserve(s);
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

bool PostZoneDataContainer::PolyDataLoader::load(std::map<std::string, vtkPointSetAndValueRangeSetT<vtkPolyData>*>* polyDataMap, std::map<std::string, std::vector<int> >* polyDataCellIdsMap, iRICLib::H5CgnsPolyDataSolution* sol, const QPointF& offset)
{
	for (const auto& pair : *polyDataMap) {
		delete pair.second;
	}
	polyDataMap->clear();

	polyDataCellIdsMap->clear();

	if (sol == nullptr) {
		return true;
	}

	std::vector<std::string> groupNames;
	int ier = sol->readGroupNames(&groupNames);
	if (ier != 0) {return false;}

	for (auto name : groupNames) {
		auto polyData = vtkPolyData::New();
		std::vector<int> ids;
		int ret = loadPolyData(name, polyData, &ids, sol, offset);
		if (ret != 0) {return false;}

		auto data = new vtkPointSetAndValueRangeSetT<vtkPolyData>(polyData);

		polyDataMap->insert({name, data});
		polyDataCellIdsMap->insert({name, ids});
	}

	return true;
}
