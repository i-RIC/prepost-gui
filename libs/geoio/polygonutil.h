#ifndef POLYGONUTIL_H
#define POLYGONUTIL_H

#include "geoio_global.h"

#include <map>
#include <vector>

namespace geos {
namespace geom {
class LinearRing;
class LineString;
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
	static geos::geom::Polygon* getGeosPolygon(const QPolygonF& region, const std::vector<QPolygonF>& holes);
	static geos::geom::LinearRing* getLinearRing(const QPolygonF& polygon);

	static void triangulate(const QPolygonF& polygon, std::vector<unsigned int>* indices);
	static void triangulate(const QPolygonF& region, const std::vector<QPolygonF>& holes, std::vector<QPointF>* outPoints, std::vector<unsigned int>* indices);
	static void triangulate(geos::geom::Polygon* polygon, std::vector<QPointF>* points, std::vector<unsigned int>* indices);
	static void triangulateTriangle(geos::geom::Polygon* polygon, std::vector<QPointF>* points, std::vector<unsigned int>* indices);
	static void triangulateVtk(geos::geom::Polygon* polygon, std::vector<QPointF>* points, std::vector<unsigned int>* indices);

	static void triangleAddPointsAndSegments(const geos::geom::LinearRing* ring, std::vector<double>* pointList, std::vector<int>* segmentList, std::map<QPointF, unsigned int>* pointMap, const QPointF& offset);
	static void triangleAddHole(const geos::geom::LinearRing* ring, std::vector<double>* holeList);

	static void triangulateVtk(const QPolygonF& polygon, std::vector<unsigned int>* indices);
	static void triangulateVtk(const QPolygonF& polygon, std::vector<unsigned int>* indices, vtkPolygon* pol, vtkIdList* triIds);

private:
	PolygonUtil();
};

#endif // POLYGONUTIL_H
