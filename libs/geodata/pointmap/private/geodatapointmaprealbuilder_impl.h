#ifndef GEODATAPOINTMAPREALBUILDER_IMPL_H
#define GEODATAPOINTMAPREALBUILDER_IMPL_H

#include "../geodatapointmaprealbuilder.h"

class vtkPoints;
class vtkDoubleArray;

class GeoDataPointmapRealBuilder::Impl
{
public:
	Impl();
	~Impl();

	vtkPoints* m_points;
	vtkDoubleArray* m_values;
};

#endif // GEODATAPOINTMAPREALBUILDER_IMPL_H
