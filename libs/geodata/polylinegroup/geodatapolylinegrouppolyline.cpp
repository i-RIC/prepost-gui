#include "geodatapolylinegroup.h"
#include "geodatapolylinegrouppolyline.h"
#include "private/geodatapolylinegrouppolyline_impl.h"

#include <geodata/polyline/geodatapolyline.h>
#include <misc/mathsupport.h>

#include <geos/geom/Coordinate.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Envelope.h>
#include <geos/geom/LineString.h>
#include <geos/geom/LinearRing.h>
#include <geos/geom/Point.h>
#include <geos/geom/Polygon.h>

#include <QDataStream>
#include <QList>
#include <QPointF>
#include <QVector>

using namespace geos::geom;

namespace {

geos::geom::LineString* buildString(const QVector<QPointF>& line)
{
	CoordinateSequence *seq = new CoordinateArraySequence(line.size());
	int idx = 0;
	for (const auto& p : line) {
		seq->setAt(Coordinate(p.x(), p.y()), idx ++);
	}
	auto f = geos::geom::GeometryFactory::getDefaultInstance();
	return f->createLineString(seq);
}

geos::geom::LineString* buildString(const std::vector<QPointF>& line)
{
	CoordinateSequence *seq = new CoordinateArraySequence(line.size());
	int idx = 0;
	for (const auto& p : line) {
		seq->setAt(Coordinate(p.x(), p.y()), idx ++);
	}
	auto f = geos::geom::GeometryFactory::getDefaultInstance();
	return f->createLineString(seq);
}

geos::geom::LinearRing* buildRing(const QPolygonF& polygon)
{
	CoordinateSequence *seq = new CoordinateArraySequence(polygon.size());
	int idx = 0;
	for (const auto& p : polygon) {
		seq->setAt(Coordinate(p.x(), p.y()), idx ++);
	}
	auto f = geos::geom::GeometryFactory::getDefaultInstance();
	return f->createLinearRing(seq);
}

geos::geom::Polygon* buildPolygon(const QPolygonF& polygon)
{
	auto outerRing = buildRing(polygon);
	std::vector<Geometry*>* holeRings = new std::vector<Geometry*>();
	auto f = geos::geom::GeometryFactory::getDefaultInstance();
	return f->createPolygon(outerRing, holeRings);
}

QVector<QPointF> buildPolyLine(const geos::geom::LineString* string)
{
	QVector<QPointF> ret(string->getNumPoints());
	for (int i = 0; i < string->getNumPoints(); ++i) {
		const auto& coords = string->getCoordinateN(i);
		ret[i] = QPointF(coords.x, coords.y);
	}
	return ret;
}

geos::geom::LineString* applyOffset(const geos::geom::LineString* string, double x, double y)
{
	auto pol = buildPolyLine(string);
	for (auto& p : pol) {
		p -= QPointF(x, y);
	}
	return buildString(pol);
}

} // namespace

GeoDataPolyLineGroupPolyLine::GeoDataPolyLineGroupPolyLine(GeoDataPolyLineGroup* group) :
	GeoDataPolyDataGroupPolyDataWithBoundingRect(group),
	impl {new Impl {}}
{}

GeoDataPolyLineGroupPolyLine::GeoDataPolyLineGroupPolyLine(const std::vector<QPointF>& line, GeoDataPolyLineGroup* group) :
	GeoDataPolyLineGroupPolyLine(group)
{
	impl->m_polyLine.reset(buildString(line));

	setupBoundingRect();
}

GeoDataPolyLineGroupPolyLine::GeoDataPolyLineGroupPolyLine(geos::geom::LineString* lineString, GeoDataPolyLineGroup* group) :
	GeoDataPolyLineGroupPolyLine(group)
{
	impl->m_polyLine.reset(lineString);

	setupBoundingRect();
}

GeoDataPolyLineGroupPolyLine::~GeoDataPolyLineGroupPolyLine()
{
	delete impl;
}

bool GeoDataPolyLineGroupPolyLine::intersectWithLine(const QPointF& p1, const QPointF& p2, QPointF* intersection, double* r) const
{
	auto tmpPoints = points();

	double s;
	for (int i = 0; i < tmpPoints.size() - 1; ++i) {
		QPointF q1 = tmpPoints.at(i);
		QPointF q2 = tmpPoints.at(i + 1);

		bool ok = iRIC::intersectionPoint(p1, p2, q1, q2, intersection, r, &s);
		if (! ok) {continue;}
		if (*r < 0 || *r > 1) {continue;}
		if (s < 0 || s > 1) {continue;}

		return true;
	}
	return false;
}

bool GeoDataPolyLineGroupPolyLine::intersectWithPolygon(const QPolygonF& polygon) const
{
	auto pol = buildPolygon(polygon);
	bool hasIntersection = pol->intersects(impl->m_polyLine.get());

	delete pol;
	return hasIntersection;
}

geos::geom::LineString* GeoDataPolyLineGroupPolyLine::getGeosLineString() const
{
	return impl->m_polyLine.get();
}

void GeoDataPolyLineGroupPolyLine::setGeosLineString(geos::geom::LineString* ls)
{
	impl->m_polyLine.reset(ls);
	setupBoundingRect();
}

std::vector<QPointF> GeoDataPolyLineGroupPolyLine::points() const
{
	std::vector<QPointF> ret;
	geos::geom::LineString* ls = impl->m_polyLine.get();

	for (int i = 0; i < ls->getNumPoints(); ++i) {
		const auto& c = ls->getCoordinateN(i);
		ret.push_back(QPointF(c.x, c.y));
	}

	return ret;
}

std::vector<unsigned int> GeoDataPolyLineGroupPolyLine::lineEdges() const
{
	std::vector<unsigned int> ret;
	geos::geom::LineString* ls= impl->m_polyLine.get();

	unsigned int offset = 0;
	for (int i = 0; i < ls->getNumPoints() - 1; ++i) {
		ret.push_back(offset + i);
		ret.push_back(offset + (i + 1));
	}
	return ret;
}

void GeoDataPolyLineGroupPolyLine::copyShapeFrom(GeoDataPolyData* data)
{
	auto l = dynamic_cast<GeoDataPolyLine*> (data);
	setGeosLineString(l->getGeosLineString());
}

void GeoDataPolyLineGroupPolyLine::copyShapeTo(GeoDataPolyData* data)
{
	auto l = dynamic_cast<GeoDataPolyLine*> (data);
	l->setShape(getGeosLineString());
	l->setMouseEventMode(GeoDataPolyLine::meNormal);
	l->updateActionStatus();
}

void GeoDataPolyLineGroupPolyLine::loadExternalData(QDataStream* stream)
{
	QString id;
	QString name;
	QVariant value;

	*stream >> id >> name >> value;
	setId(id);
	setName(name);
	setValue(value);

	QVector<QPointF> points;
	*stream >> points;
	impl->m_polyLine.reset(buildString(points));

	setupBoundingRect();
}

void GeoDataPolyLineGroupPolyLine::saveExternalData(QDataStream* stream)
{
	*stream << id();
	*stream << name();
	*stream << value();
	geos::geom::LineString* ls = impl->m_polyLine.get();

	auto vec = buildPolyLine(ls);
	*stream << vec;
}

void GeoDataPolyLineGroupPolyLine::applyOffset(double x, double y)
{
	auto ls = impl->m_polyLine.get();

	impl->m_polyLine.reset(::applyOffset(ls, x, y));
	setupBoundingRect();
}

GeoDataPolyDataGroupPolyData* GeoDataPolyLineGroupPolyLine::copy(GeoDataPolyDataGroup* group) const
{
	auto ret = new GeoDataPolyLineGroupPolyLine(dynamic_cast<GeoDataPolyLineGroup*> (group));
	ret->setName(name());
	ret->setGeosLineString(dynamic_cast<geos::geom::LineString*> (getGeosLineString()->clone()));
	ret->setValue(value());

	return ret;
}

void GeoDataPolyLineGroupPolyLine::applyShape()
{
	auto l = dynamic_cast<GeoDataPolyLine*> (sender());
	if (l == nullptr) {return;}

	setGeosLineString(l->getGeosLineString());
	emit modified();
}

void GeoDataPolyLineGroupPolyLine::setupBoundingRect()
{
	const auto env = impl->m_polyLine.get()->getEnvelopeInternal();

	m_xmin = env->getMinX();
	m_xmax = env->getMaxX();
	m_ymin = env->getMinY();
	m_ymax = env->getMaxY();
}
