#ifndef GEODATAPOLYGONREGIONPOLYGON_H
#define GEODATAPOLYGONREGIONPOLYGON_H

#include "gd_polygon_global.h"
#include "geodatapolygonabstractpolygon.h"

class GD_POLYGON_EXPORT GeoDataPolygonRegionPolygon : public GeoDataPolygonAbstractPolygon
{
	Q_OBJECT

public:
	GeoDataPolygonRegionPolygon(GeoDataPolygon* parent);
};

#endif // GEODATAPOLYGONREGIONPOLYGON_H
