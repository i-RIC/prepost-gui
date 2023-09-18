#include "vtkstructuredgridextended3d.h"

vtkStructuredGridExtended3d::vtkStructuredGridExtended3d() :
	vtkPointSetExtendedT<vtkStructuredGrid> {vtkStructuredGrid::New(), false, false}
{}

vtkStructuredGrid* vtkStructuredGridExtended3d::concreteData() const
{
	return vtkPointSetExtendedT<vtkStructuredGrid>::concreteData();
}
