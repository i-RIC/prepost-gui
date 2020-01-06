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

#include <cgnslib.h>
#include <iriclib.h>

#include <sstream>
#include <vector>

namespace {

int loadPolyData(const std::string& name, vtkPolyData* polyData, std::vector<int>* ids, const QPointF& offset)
{
	std::vector<int> typeVec;
	std::vector<int> sizeVec;
	std::vector<double> coordXVec;
	std::vector<double> coordYVec;

	std::vector<int> cellCounts;
	std::vector<int> cellIntVals;
	std::vector<double> cellRealVals;

	CgnsUtil::loadArrayWithName(name, "type", &typeVec);
	CgnsUtil::loadArrayWithName(name, "size", &sizeVec);
	CgnsUtil::loadArrayWithName(name, "coordinateX", &coordXVec);
	CgnsUtil::loadArrayWithName(name, "coordinateY", &coordYVec);

	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
	vtkSmartPointer<vtkCellArray> polys = vtkSmartPointer<vtkCellArray>::New();

	int startIdx = 0;
	for (int i = 0; i < typeVec.size(); ++i) {
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
			vtkSmartPointer<vtkIdList> triIds = vtkSmartPointer<vtkIdList>::New();
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
	int numArray;

	auto exp = QString("^%1__(.+)$").arg(name.c_str());
	QRegExp re(exp);

	cg_narrays(&numArray);
	for (int A = 1; A <= numArray; ++A) {
		int ier;
		char n[ProjectCgnsFile::BUFFERLEN];
		DataType_t dataType;
		int dim;
		cgsize_t dimVec;

		ier = cg_array_info(A, n, &dataType, &dim, &dimVec);
		if (ier != 0) {return ier;}
		if (re.indexIn(n) == -1) {continue;}

		vtkSmartPointer<vtkDoubleArray> vals = vtkSmartPointer<vtkDoubleArray>::New();
		vals->SetName(iRIC::toStr(re.cap(1)).c_str());

		if (dataType == Integer) {
			cellIntVals.assign(dimVec, 0);
			cg_array_read(A, cellIntVals.data());
			for (int j = 0; j < cellIntVals.size(); ++j) {
				for (int k = 0; k < cellCounts[j]; ++k) {
					vals->InsertNextValue(cellIntVals.at(j));
				}
			}
		} else {
			cellRealVals.assign(dimVec, 0);
			cg_array_read(A, cellRealVals.data());
			for (int j = 0; j < cellRealVals.size(); ++j) {
				for (int k = 0; k < cellCounts[j]; ++k) {
					vals->InsertNextValue(cellRealVals.at(j));
				}
			}
		}
		polyData->GetCellData()->AddArray(vals);
	}

	return 0;
}

std::vector<std::string> loadPolyDataNames()
{
	int ier;
	int numArrays;
	std::vector<std::string> names;

	ier = cg_narrays(&numArrays);
	QRegExp re("^(.+)_type$");

	for (int A = 1; A <= numArrays; ++A) {
		char name[ProjectCgnsFile::BUFFERLEN];
		DataType_t dataType;
		int dim;
		cgsize_t dimVec;

		ier = cg_array_info(A, name, &dataType, &dim, &dimVec);
		int idx = re.indexIn(name);
		if (idx == 0) {
			names.push_back(iRIC::toStr(re.cap(1)));
		}
	}

	return names;
}

} // namespace

bool PostZoneDataContainer::PolyDataLoader::load(int fid, int bid, int zid, int step, std::map<std::string, vtkSmartPointer<vtkPolyData> >* polyDataMap, std::map<std::string, std::vector<int> >* polyDataCellIdsMap, const QPointF& offset)
{
	polyDataMap->clear();
	std::ostringstream ss;
	ss << "PolydataSolution" << (step + 1);
	int ier = cg_goto(fid, bid, "Zone_t", zid, ss.str().c_str(), 0, "end");
	if (ier != 0) {return true;}

	auto names = loadPolyDataNames();
	for (auto name : names) {
		vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
		std::vector<int> ids;
		int ret = loadPolyData(name, polyData.Get(), &ids, offset);
		if (ret != 0) {return false;}

		polyDataMap->insert({name, polyData});
		polyDataCellIdsMap->insert({name, ids});
	}
	return true;
}
