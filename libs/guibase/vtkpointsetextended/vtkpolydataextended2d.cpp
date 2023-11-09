#include "vtkpolydataextended2d.h"

vtkPolyDataExtended2d::vtkPolyDataExtended2d() :
	vtkPointSetExtendedT<vtkPolyData> {vtkPolyData::New(), true, true}
{}

vtkPolyData* vtkPolyDataExtended2d::concreteData() const
{
	return vtkPointSetExtendedT<vtkPolyData>::concreteData();
}
