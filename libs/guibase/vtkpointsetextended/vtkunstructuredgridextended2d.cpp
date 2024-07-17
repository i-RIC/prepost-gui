#include "vtkunstructuredgridextended2d.h"

vtkUnstructuredGridExtended2d::vtkUnstructuredGridExtended2d() :
	vtkPointSetExtendedT<vtkUnstructuredGrid> {vtkUnstructuredGrid::New(), true, true}
{}

vtkUnstructuredGrid* vtkUnstructuredGridExtended2d::concreteData() const
{
	return vtkPointSetExtendedT<vtkUnstructuredGrid>::concreteData();
}
