#ifndef GEODATAPOLYGONHOLEPOLYGON_H
#define GEODATAPOLYGONHOLEPOLYGON_H

#include "geodatapolygonabstractpolygon.h"

class GeoDataPolygonHolePolygon : public GeoDataPolygonAbstractPolygon
{
	Q_OBJECT

public:
	GeoDataPolygonHolePolygon(GeoDataPolygon* parent);
};

#endif // GEODATAPOLYGONHOLEPOLYGON_H
