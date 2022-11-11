#include "vtkpolydatapointsactor.h"

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

vtkPolyDataPointsActor::vtkPolyDataPointsActor() :
	vtkActorPolyDataMapperPair {}
{
	actor()->GetProperty()->SetRepresentationToPoints();
	actor()->GetProperty()->SetColor(0, 0, 0);
}

void vtkPolyDataPointsActor::setPolyData(vtkPolyData* polyData)
{
	mapper()->SetInputData(polyData);
}
