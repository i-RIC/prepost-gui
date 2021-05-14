#include "../../misc/cgnsutil.h"
#include "postzonedatacontainer_particlegrouploader.h"

#include <QPointF>

#include <vtkCellArray.h>
#include <vtkCellData.h>
#include <vtkPoints.h>

#include <h5cgnsbase.h>
#include <h5cgnsparticlegroupsolution.h>
#include <public/h5cgnsparticlegroupsolution_groupreader.h>
#include <h5cgnszone.h>

#include <vector>

bool PostZoneDataContainer::ParticleGroupLoader::load(std::map<std::string, vtkSmartPointer<vtkPolyData> >* particleGroupMap, iRICLib::H5CgnsParticleGroupSolution* sol, const QPointF& offset)
{
	particleGroupMap->clear();

	std::vector<std::string> groupNames;
	int ier = sol->readGroupNames(&groupNames);
	if (ier != 0) {return false;}

	for (auto name : groupNames) {
		auto polyData = vtkSmartPointer<vtkPolyData>::New();
		int ret = loadParticleGroup(name, polyData.Get(), sol, offset);
		if (ret != 0) {return false;}

		particleGroupMap->insert({name, polyData});
	}
	return true;
}

int PostZoneDataContainer::ParticleGroupLoader::loadParticleGroup(const std::string& name, vtkPolyData* polyData, iRICLib::H5CgnsParticleGroupSolution* sol, const QPointF& offset)
{
	std::vector<double> coordXVec;
	std::vector<double> coordYVec;
	std::vector<double> coordZVec;

	std::vector<int> cellIntVals;
	std::vector<double> cellRealVals;

	int ier;
	ier = sol->readCoordinatesX(name, &coordXVec);
	if (ier != 0) {return ier;}

	ier = sol->readCoordinatesY(name, &coordYVec);
	if (ier != 0) {return ier;}
	if (sol->zone()->base()->dimension() == 3) {
		ier = sol->readCoordinatesZ(name, &coordZVec);
		if (ier != 0) {return ier;}
	} else {
		coordZVec.assign(coordXVec.size(), 0);
	}

	auto points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	vtkSmartPointer<vtkCellArray> verts = vtkSmartPointer<vtkCellArray>::New();

	for (unsigned int i = 0; i < coordXVec.size(); ++i) {
		points->InsertNextPoint(coordXVec.at(i) - offset.x(), coordYVec.at(i) - offset.y(), coordZVec.at(i));
		vtkIdType pId = i;
		verts->InsertNextCell(1, &pId);
	}
	polyData->SetPoints(points);
	polyData->SetVerts(verts);

	// load values
	auto reader = sol->groupReader(name);

	auto pd = polyData->GetPointData();

	CgnsUtil::loadScalarData(&reader, pd);
	CgnsUtil::loadVectorData(&reader, pd);

	return 0;
}
