#ifndef POLYGONUTIL_H
#define POLYGONUTIL_H

#include "geoio_global.h"

#include <vector>

namespace geos {
namespace geom {
class Polygon;
} // geom
} // geos


class QPointF;
class QPolygonF;

class GEOIODLL_EXPORT PolygonUtil
{
public:
	static std::vector<QPointF> toStdVector(const QPolygonF& polygon);
	static QPolygonF fromStdVector(const std::vector<QPointF>& points);

	static geos::geom::Polygon* getGeosPolygon(const QPolygonF& polygon);

private:
	PolygonUtil();
};

#endif // POLYGONUTIL_H
