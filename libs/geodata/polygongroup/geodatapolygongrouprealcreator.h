#ifndef GEODATAPOLYGONGROUPREALCREATOR_H
#define GEODATAPOLYGONGROUPREALCREATOR_H

#include "geodatapolygongroupcreatort.h"

#include <vtkDoubleArray.h>

class GD_POLYGONGROUP_EXPORT GeoDataPolygonGroupRealCreator : public GeoDataPolygonGroupCreatorT<double, vtkDoubleArray>
{
public:
	GeoDataPolygonGroupRealCreator();

protected:
	GeoDataPolygonGroupRealCreator(const QString& typeName);
};

#endif // GEODATAPOLYGONGROUPREALCREATOR_H
