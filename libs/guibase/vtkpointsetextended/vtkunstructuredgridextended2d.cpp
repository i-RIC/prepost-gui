#include "vtkunstructuredgridextended2d.h"

vtkUnstructuredGridExtended2d::vtkUnstructuredGridExtended2d() :
	vtkPointSetExtendedT<vtkUnstructuredGrid> {vtkUnstructuredGrid::New(), false, false}
{}

vtkUnstructuredGrid* vtkUnstructuredGridExtended2d::concreteData() const
{
	return vtkPointSetExtendedT<vtkUnstructuredGrid>::concreteData();
}
