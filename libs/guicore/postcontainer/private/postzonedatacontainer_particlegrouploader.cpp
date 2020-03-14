#include "../../misc/cgnsutil.h"
#include "../../project/projectcgnsfile.h"
#include "postzonedatacontainer_particlegrouploader.h"

#include <misc/stringtool.h>

#include <QPointF>
#include <QRegExp>

#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkDoubleArray.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkPoints.h>

#include <cgnslib.h>

#include <sstream>
#include <vector>

namespace {

std::vector<std::string> loadParticleGroupNames()
{
	int ier;
	int numArrays;
	std::vector<std::string> names;

	ier = cg_narrays(&numArrays);
	QRegExp re("^(.+)_coordinateX$");

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

bool PostZoneDataContainer::ParticleGroupLoader::load(int fid, int bid, int zid, int step, std::map<std::string, vtkSmartPointer<vtkPolyData> >* particleGroupMap, const QPointF& offset)
{
	particleGroupMap->clear();
	std::ostringstream ss;
	ss << "ParticleGroupSolution" << (step + 1);
	int ier = cg_goto(fid, bid, "Zone_t", zid, ss.str().c_str(), 0, "end");
	if (ier != 0) {return true;}

	auto names = loadParticleGroupNames();
	for (auto name : names) {
		vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
		int ret = loadParticleGroup(name, polyData.Get(), offset);
		if (ret != 0) {return false;}

		particleGroupMap->insert({name, polyData});
	}
	return true;
}

bool PostZoneDataContainer::ParticleGroupLoader::loadScalarData(vtkDataSetAttributes* atts, const QString& groupName)
{
	int narrays;
	cg_narrays(&narrays);
	for (int i = 1; i <= narrays; ++i) {
		DataType_t datatype;
		int dimension;
		cgsize_t dimVector[3];
		char arrayname[ProjectCgnsFile::BUFFERLEN];
		cg_array_info(i, arrayname, &datatype, &dimension, dimVector);
		QString name(arrayname);

		if (name.left(groupName.length()) != groupName) {continue;}
		if (name.contains("_coordinate")) {continue;}

		// skip vector values.
		QRegExp rx;
		// For example, "VelocityX", "VelocityY", "VelocityZ"
		rx = QRegExp("(.*)(X|Y|Z)$");
		if (rx.indexIn(name) != -1) {continue;}

		int datalen = 1;
		for (int j = 0; j < dimension; ++j) {
			datalen *= dimVector[j];
		}
		QString valueName = name.right(name.length() - groupName.length() - 2);

		if (datatype == Integer) {
			CgnsUtil::loadScalarDataT<int, vtkIntArray>(valueName, atts, i, datalen);
		} else if (datatype == RealSingle) {
			CgnsUtil::loadScalarDataT<float, vtkFloatArray>(valueName, atts, i, datalen);
		} else if (datatype == RealDouble) {
			CgnsUtil::loadScalarDataT<double, vtkDoubleArray>(valueName, atts, i, datalen);
		}
	}
	atts->Modified();
	return true;
}


bool PostZoneDataContainer::ParticleGroupLoader::loadVectorData(vtkDataSetAttributes* atts, const QString& groupName)
{
	int narrays;
	cg_narrays(&narrays);
	for (int i = 1; i <= narrays; ++i) {
		DataType_t datatype;
		int dimension;
		cgsize_t dimVector[3];
		char arrayname[ProjectCgnsFile::BUFFERLEN];
		cg_array_info(i, arrayname, &datatype, &dimension, dimVector);
		QString name(arrayname);

		if (name.left(groupName.length()) != groupName) {continue;}
		if (name.contains("_coordinate")) {continue;}

		// skip vector values.
		QRegExp rx;
		// For example, "VelocityX", "VelocityY", "VelocityZ"
		rx = QRegExp("(.*)X$");
		if (rx.indexIn(name) == -1) {continue;}

		int indexX, indexY, indexZ;
		// vector data found.
		QString cap = rx.cap(1);
		QString vectorName = cap.right(cap.length() - groupName.length() - 2);
		QString baseName = cap.right(cap.length());

		indexX = i;
		indexY = 0;
		indexZ = 0;

		QString yname = baseName;
		yname.append("Y");

		std::string ynamestr = iRIC::toStr(yname);

		indexY = CgnsUtil::findArray(yname, datatype, dimension, narrays);
		if (indexY != 0) {
			// try to find Z component
			QString zname = baseName;
			zname.append("Z");
			indexZ = CgnsUtil::findArray(zname, datatype, dimension, narrays);
		}
		// indexX, indexY, indexZ are set correctly.
		int datalen = 1;
		for (int j = 0; j < dimension; ++j) {
			datalen *= dimVector[j];
		}
		if (datatype == Integer) {
			CgnsUtil::loadVectorDataT<int, vtkIntArray> (vectorName, atts, indexX, indexY, indexZ, datalen);
		} else if (datatype == RealSingle) {
			CgnsUtil::loadVectorDataT<float, vtkFloatArray> (vectorName, atts, indexX, indexY, indexZ, datalen);
		} else if (datatype == RealDouble) {
			CgnsUtil::loadVectorDataT<double, vtkDoubleArray> (vectorName, atts, indexX, indexY, indexZ, datalen);
		}
	}
	atts->Modified();
	return true;
}

int PostZoneDataContainer::ParticleGroupLoader::loadParticleGroup(const std::string& name, vtkPolyData* polyData, const QPointF& offset)
{
	std::vector<double> coordXVec;
	std::vector<double> coordYVec;
	std::vector<double> coordZVec;

	std::vector<int> cellIntVals;
	std::vector<double> cellRealVals;

	CgnsUtil::loadArrayWithName(name, "coordinateX", &coordXVec);
	CgnsUtil::loadArrayWithName(name, "coordinateY", &coordYVec);
	CgnsUtil::loadArrayWithName(name, "coordinateZ", &coordZVec);

	if (coordZVec.size() == 0) {
		coordZVec.assign(coordXVec.size(), 0);
	}

	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	vtkSmartPointer<vtkCellArray> verts = vtkSmartPointer<vtkCellArray>::New();

	for (int i = 0; i < coordXVec.size(); ++i) {
		points->InsertNextPoint(coordXVec.at(i) - offset.x(), coordYVec.at(i) - offset.y(), coordZVec.at(i));
		vtkIdType pId = i;
		verts->InsertNextCell(1, &pId);
	}
	polyData->SetPoints(points);
	polyData->SetVerts(verts);

	// load values
	loadScalarData(polyData->GetPointData(), name.c_str());
	loadVectorData(polyData->GetPointData(), name.c_str());

	return 0;
}
