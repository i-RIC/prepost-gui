#include "vtkstructuredgridextended2d.h"

vtkStructuredGridExtended2d::vtkStructuredGridExtended2d() :
	vtkPointSetExtendedT<vtkStructuredGrid> {vtkStructuredGrid::New(), true, true}
{}

vtkStructuredGrid* vtkStructuredGridExtended2d::concreteData() const
{
	return vtkPointSetExtendedT<vtkStructuredGrid>::concreteData();
}
