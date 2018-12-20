#ifndef GEODATAPOLYLINEIMPLPOLYLINE_H
#define GEODATAPOLYLINEIMPLPOLYLINE_H

#include "gd_polyline_global.h"
#include "geodatapolylineabstractpolyline.h"

class GD_POLYLINE_EXPORT GeoDataPolyLineImplPolyLine : public GeoDataPolyLineAbstractPolyLine
{
	Q_OBJECT

public:
	GeoDataPolyLineImplPolyLine(GeoDataPolyLine* parent);
};

#endif // GEODATAPOLYLINEIMPLPOLYLINE_H
