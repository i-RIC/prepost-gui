#include "postzonedatacontainer_particleloader.h"
#include "../../misc/cgnsutil.h"
#include "../../project/projectcgnsfile.h"

#include <QPointF>

#include <vtkCellArray.h>
#include <vtkPointData.h>

#include <h5cgnsbase.h>
#include <h5cgnsparticlesolution.h>
#include <h5cgnszone.h>

bool PostZoneDataContainer::ParticleLoader::load(vtkSmartPointer<vtkPolyData>* data, iRICLib::H5CgnsParticleSolution* sol, const QPointF& offset)
{
	if (data->Get() == nullptr) {
		(*data) = vtkSmartPointer<vtkPolyData>::New();
	} else {
		(*data)->Initialize();
	}

	int numParticles;
	int ier = sol->count(&numParticles);
	if (ier != 0) {return false;}

	std::vector<double> dataX, dataY, dataZ;

	ier = sol->readCoordinatesX(&dataX);
	if (ier != 0) {return false;}

	ier = sol->readCoordinatesY(&dataY);
	if (ier != 0) {return false;}

	if (sol->zone()->base()->dimension() == 3) {
		ier = sol->readCoordinatesZ(&dataZ);
		if (ier != 0) {return false;}
	} else {
		dataZ.assign(dataX.size(), 0);
	}

	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> verts = vtkSmartPointer<vtkCellArray>::New();
	for (int i = 0; i < numParticles; ++i) {
		points->InsertNextPoint(dataX[i] - offset.x(), dataY[i] - offset.y(), dataZ[i]);
		vtkIdType pId = i;
		verts->InsertNextCell(1, &pId);
	}
	(*data)->SetPoints(points);
	(*data)->SetVerts(verts);

	vtkPointData* pd = (*data)->GetPointData();

	CgnsUtil::loadScalarData(sol, pd);
	CgnsUtil::loadVectorData(sol, pd);

	(*data)->Modified();

	return true;
}
