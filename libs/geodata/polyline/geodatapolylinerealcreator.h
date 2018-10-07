#ifndef GEODATAPOLYLINEREALCREATOR_H
#define GEODATAPOLYLINEREALCREATOR_H

#include "geodatapolylinecreatort.h"
#include <vtkDoubleArray.h>

class GD_POLYLINE_EXPORT GeoDataPolyLineRealCreator : public GeoDataPolyLineCreatorT<double, vtkDoubleArray>
{
public:
	GeoDataPolyLineRealCreator();
};

#endif // GEODATAPOLYLINEREALCREATOR_H
