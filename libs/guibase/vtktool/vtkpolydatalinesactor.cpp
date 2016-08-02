#include "vtkpolydatalinesactor.h"

#include <vtkActor.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

vtkPolyDataLinesActor::vtkPolyDataLinesActor() :
	vtkActorPolyDataMapperPair {}
{
	actor()->GetProperty()->SetRepresentationToWireframe();
}

void vtkPolyDataLinesActor::setPolyData(vtkPolyData* polyData)
{
	mapper()->SetInputData(polyData);
}
