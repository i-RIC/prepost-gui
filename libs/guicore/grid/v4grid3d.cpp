#include "v4grid3d.h"

v4Grid3d::v4Grid3d(vtkPointSetExtended* data) :
	v4Grid {data}
{}

vtkPointSet* v4Grid3d::vtkIndexData() const
{
	return nullptr;
}
