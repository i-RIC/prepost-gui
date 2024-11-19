#include "v4grid1d.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>

v4Grid1d::v4Grid1d(vtkPointSetExtended* data) :
	v4Grid {data}
{}

v4Grid1d::~v4Grid1d()
{}

double v4Grid1d::point1d(vtkIdType index) const
{
	double p[3];
	vtkData()->data()->GetPoints()->SetPoint(index, p);
	return p[0];
}

void v4Grid1d::setPoint1d(vtkIdType index, double v)
{
	vtkData()->data()->GetPoints()->SetPoint(index, v, 0, 0);
}
