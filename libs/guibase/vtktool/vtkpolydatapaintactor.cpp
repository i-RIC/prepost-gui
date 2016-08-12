#include "vtkpolydatapaintactor.h"

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

vtkPolyDataPaintActor::vtkPolyDataPaintActor() :
	vtkActorPolyDataMapperPair {}
{
	actor()->GetProperty()->SetRepresentationToSurface();
}

void vtkPolyDataPaintActor::setPolyData(vtkPolyData* polyData)
{
	mapper()->SetInputData(polyData);
}
