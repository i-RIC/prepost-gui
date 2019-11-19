#include "polygonutil.h"

#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/GeometryFactory.h>

#include <QPolygonF>

std::vector<QPointF> PolygonUtil::toStdVector(const QPolygonF& polygon)
{
	std::vector<QPointF> ret;
	for (QPointF p : polygon) {
		ret.push_back(p);
	}

	return ret;
}

QPolygonF PolygonUtil::fromStdVector(const std::vector<QPointF>& points)
{
	QPolygonF ret;
	for (QPointF p : points) {
		ret.push_back(p);
	}

	return ret;
}

geos::geom::Polygon* PolygonUtil::getGeosPolygon(const QPolygonF& polygon)
{
	auto f = geos::geom::GeometryFactory::getDefaultInstance();
	const auto csf = f->getCoordinateSequenceFactory();
	auto cs = csf->create(polygon.size(), 2);
	for (unsigned int i = 0; i < polygon.size(); ++i) {
		auto p = polygon.at(i);
		geos::geom::Coordinate c(p.x(), p.y());
		cs->setAt(c, i);
	}
	auto linearRing = f->createLinearRing(cs);
	auto holes = new std::vector<geos::geom::Geometry*>();
	return f->createPolygon(linearRing, holes);
}

PolygonUtil::PolygonUtil()
{}
