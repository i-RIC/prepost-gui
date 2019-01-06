#include "postzonedatacontainer_particleloader.h"
#include "../../project/projectcgnsfile.h"

#include <QPointF>

#include <vtkCellArray.h>
#include <vtkPointData.h>

#include <cgnslib.h>

#include <sstream>

bool PostZoneDataContainer::ParticleLoader::load(int fid, int bid, int zid, int step, vtkSmartPointer<vtkPolyData>* data, const QPointF& offset)
{
	// Find ParticleResult node.
	std::ostringstream ss;
	ss << "ParticleSolution" << (step + 1);
	int ier = cg_goto(fid, bid, "Zone_t", zid, ss.str().c_str(), 0, "end");
	if (ier != 0) {
		// particle data does not exists.
		// and it is not an error, because particle is optional data.
		(*data) = nullptr;
		return true;
	}
	if (data->Get() == nullptr) {
		(*data) = vtkSmartPointer<vtkPolyData>::New();
	} else {
		(*data)->Initialize();
	}

	// get array info in order to know the number of particles.
	char aName[ProjectCgnsFile::BUFFERLEN];
	DataType_t dType;
	int d;
	cgsize_t dVector[3];

	// get the number of particles.
	cg_array_info(1, aName, &dType, &d, dVector);

	size_t numParticles = dVector[0];
	std::vector<double> dataX(numParticles, 0);
	std::vector<double> dataY(numParticles, 0);
	std::vector<double> dataZ(numParticles, 0);

	int firstAttId = 4;

	// Read X
	ier = cg_array_info(1, aName, &dType, &d, dVector);
	if (ier != 0 || QString(aName) != "CoordinateX" || dVector[0] != numParticles) {
		return false;
	}
	ier = cg_array_read_as(1, RealDouble, dataX.data());
	if (ier != 0) {return false;}

	// Read Y
	ier = cg_array_info(2, aName, &dType, &d, dVector);
	if (ier != 0 || QString(aName) != "CoordinateY" || dVector[0] != numParticles) {
		return false;
	}
	ier = cg_array_read_as(2, RealDouble, dataY.data());
	if (ier != 0) {return false;}

	// Read Z (optional)
	ier = cg_array_info(3, aName, &dType, &d, dVector);
	if (ier != 0 || QString(aName) != "CoordinateZ") {
		// Z data does not exist;
		firstAttId = 3;
	} else {
		if (dVector[0] != numParticles) {
			return false;
		}
		ier = cg_array_read_as(3, RealDouble, dataZ.data());
		if (ier != 0) {return false;}
	}

	// X, Y, Z are setup.
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> verts = vtkSmartPointer<vtkCellArray>::New();
	for (size_t i = 0; i < numParticles; ++i) {
		points->InsertNextPoint(dataX[i] - offset.x(), dataY[i] - offset.y(), dataZ[i]);
		vtkIdType pId = i;
		verts->InsertNextCell(1, &pId);
	}
	(*data)->SetPoints(points);
	(*data)->SetVerts(verts);

	vtkPointData* pd = (*data)->GetPointData();

	loadScalarData(pd, firstAttId);
	loadVectorData(pd, firstAttId);

	(*data)->Modified();

	return true;
}
