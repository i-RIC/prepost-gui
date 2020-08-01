#ifndef GEODATAPOLYLINEGROUPREALCREATOR_H
#define GEODATAPOLYLINEGROUPREALCREATOR_H

#include "geodatapolylinegroupcreatort.h"

#include <vtkDoubleArray.h>

class GD_POLYLINEGROUP_EXPORT GeoDataPolyLineGroupRealCreator : public GeoDataPolyLineGroupCreatorT<double, vtkDoubleArray>
{
public:
	GeoDataPolyLineGroupRealCreator();
};

#endif // GEODATAPOLYLINEGROUPREALCREATOR_H
