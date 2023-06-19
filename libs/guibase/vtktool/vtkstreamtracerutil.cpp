#include "vtkstreamtracerutil.h"

#include <vtkCell.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkStreamTracer.h>

void vtkStreamTracerUtil::setupForParticleTracking(vtkStreamTracer* tracer)
{
	tracer->SetIntegrationStepUnit(vtkStreamTracer::CELL_LENGTH_UNIT);
	tracer->SetMaximumPropagation(5000);
	tracer->SetComputeVorticity(false);
}

void vtkStreamTracerUtil::addParticlePointsAtTime(vtkPoints* points, vtkStreamTracer* tracer, double time)
{
	vtkPolyData* polyData = tracer->GetOutput();
	vtkPointData* pointData = polyData->GetPointData();

	vtkDoubleArray* timeArray = vtkDoubleArray::SafeDownCast(pointData->GetArray("IntegrationTime"));

	double pos[3], prevPos[3];
	double t, prevT;

	points->Resize(points->GetNumberOfPoints() + polyData->GetNumberOfCells());
	for (vtkIdType i = 0; i < polyData->GetNumberOfCells(); ++i) {
		vtkCell* cell = polyData->GetCell(i);
		for (int j = 0; j < cell->GetNumberOfPoints(); ++j) {
			vtkIdType vId = cell->GetPointId(j);

			polyData->GetPoint(vId, pos);
			t = timeArray->GetValue(vId);

			if (t > time) {
				double r = (time - prevT) / (t - prevT);
				double ipos[3]; // interpolated position
				for (int k = 0; k < 3; ++k) {
					ipos[k] = (1 - r) * prevPos[k] + r * pos[k];
				}
				points->InsertNextPoint(ipos);
				break;
			}
			for (int k = 0; k < 3; ++k) {
				prevPos[k] = pos[k];
			}
			prevT = t;
		}
	}
}
