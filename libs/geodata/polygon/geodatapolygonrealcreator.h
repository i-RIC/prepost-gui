#ifndef GEODATAPOLYGONREALCREATOR_H
#define GEODATAPOLYGONREALCREATOR_H

#include "geodatapolygoncreatort.h"
#include <vtkDoubleArray.h>

class GD_POLYGON_EXPORT GeoDataPolygonRealCreator : public GeoDataPolygonCreatorT<double, vtkDoubleArray>
{
public:
	GeoDataPolygonRealCreator();
};

#endif // GEODATAPOLYGONREALCREATOR_H
