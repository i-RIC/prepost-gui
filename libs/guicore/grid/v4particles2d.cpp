#include "v4particles2d.h"

v4Particles2d::v4Particles2d() :
	v4Grid2dT<vtkPolyDataExtended2d>()
{
	setFilteredData(vtkData()->data());
}

void v4Particles2d::updateFilteredData(double /*xMin*/, double /*xMax*/, double /*yMin*/, double /*yMax*/)
{}
