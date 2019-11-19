#ifndef GEODATAPOLYGONGROUPREALCREATOR_H
#define GEODATAPOLYGONGROUPREALCREATOR_H

#include "geodatapolygongroupcreatort.h"

#include <vtkDoubleArray.h>

class GD_POLYGONGROUP_EXPORT GeoDataPolygonGroupRealCreator : public GeoDataPolygonGroupCreatorT<double, vtkDoubleArray>
{
public:
	GeoDataPolygonGroupRealCreator();
};

#endif // GEODATAPOLYGONGROUPREALCREATOR_H
