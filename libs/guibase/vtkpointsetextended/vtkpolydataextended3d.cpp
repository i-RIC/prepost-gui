#include "vtkpolydataextended3d.h"

vtkPolyDataExtended3d::vtkPolyDataExtended3d() :
	vtkPointSetExtendedT<vtkPolyData> {vtkPolyData::New(), false, false}
{}

vtkPolyData* vtkPolyDataExtended3d::concreteData() const
{
	return vtkPointSetExtendedT<vtkPolyData>::concreteData();
}
