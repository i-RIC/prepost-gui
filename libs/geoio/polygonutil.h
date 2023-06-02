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

class vtkIdList;
class vtkPolygon;

class GEOIODLL_EXPORT PolygonUtil
{
public:
	static std::vector<QPointF> toStdVector(const QPolygonF& polygon);
	static QPolygonF fromStdVector(const std::vector<QPointF>& points);

	static geos::geom::Polygon* getGeosPolygon(const QPolygonF& polygon);
	static void triangulate(const QPolygonF& polygon, std::vector<unsigned int>* indices);
	static void triangulateTriangle(const QPolygonF& polygon, std::vector<unsigned int>* indices);
	static void triangulateVtk(const QPolygonF& polygon, std::vector<unsigned int>* indices);
	static void triangulateVtk(const QPolygonF& polygon, std::vector<unsigned int>* indices, vtkPolygon* pol, vtkIdList* triIds);

private:
	PolygonUtil();
};

#endif // POLYGONUTIL_H
