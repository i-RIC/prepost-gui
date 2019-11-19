#include "geodatapolygongrouppolygon.h"
#include "private/geodatapolygongrouppolygon_impl.h"
#include "private/geodatapolygongrouppolygon_triangulatori.h"
#include "private/geodatapolygongrouppolygon_triangulatortriangle.h"
#include "private/geodatapolygongrouppolygon_triangulatorvtk.h"

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
#include <QPolygonF>

using namespace geos::geom;

namespace {

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

QPolygonF buildPolygon(const geos::geom::LineString* string)
{
	QPolygonF ret;
	for (int i = 0; i < string->getNumPoints(); ++i) {
		const auto& coords = string->getCoordinateN(i);
		ret.push_back(QPointF(coords.x, coords.y));
	}
	return ret;
}

geos::geom::LinearRing* applyOffset(const geos::geom::LineString* string, double x, double y)
{
	auto pol = buildPolygon(string);
	for (auto& p : pol) {
		p += QPointF(x, y);
	}
	return buildRing(pol);
}

} // namespace

GeoDataPolygonGroupPolygon::GeoDataPolygonGroupPolygon() :
	impl {new Impl{}}
{
	impl->m_indexOffset = 0;
}

GeoDataPolygonGroupPolygon::GeoDataPolygonGroupPolygon(const QPolygonF& outer, const std::vector<QPolygonF>& holes) :
	impl {new Impl{}}
{
	impl->m_indexOffset = 0;

	auto outerRing = buildRing(outer);
	std::vector<Geometry*>* holeRings = new std::vector<Geometry*>();
	for (const auto& h : holes) {
		holeRings->push_back(buildRing(h));
	}

	auto f = geos::geom::GeometryFactory::getDefaultInstance();
	impl->m_polygon.reset(f->createPolygon(outerRing, holeRings));

	setupBoundingRect();
	setupTriangleCells();
}

GeoDataPolygonGroupPolygon::GeoDataPolygonGroupPolygon(geos::geom::Polygon* polygon) :
	impl {new Impl {}}
{
	impl->m_indexOffset = 0;
	impl->m_polygon.reset(polygon);

	setupBoundingRect();
	setupTriangleCells();
}

GeoDataPolygonGroupPolygon::~GeoDataPolygonGroupPolygon()
{
	delete impl;
}

QString GeoDataPolygonGroupPolygon::name() const
{
	return impl->m_name;
}

void GeoDataPolygonGroupPolygon::setName(const QString& name)
{
	impl->m_name = name;
}

QVariant GeoDataPolygonGroupPolygon::value() const
{
	return impl->m_value;
}

void GeoDataPolygonGroupPolygon::setValue(const QVariant& v)
{
	impl->m_value = v;
}

bool GeoDataPolygonGroupPolygon::isInside(const QPointF& point) const
{
	if (impl->m_polygon.get() == nullptr) {return false;}
	geos::geom::Polygon* pol = impl->m_polygon.get();

	auto f = GeometryFactory::getDefaultInstance();
	Coordinate coord(point.x(), point.y());
	auto p = std::unique_ptr<geos::geom::Point>(f->createPoint(coord));

	return (pol->contains(p.get()));
}

geos::geom::Polygon* GeoDataPolygonGroupPolygon::geosPolygon() const
{
	return impl->m_polygon.get();
}

std::vector<QPointF> GeoDataPolygonGroupPolygon::points() const
{
	std::vector<QPointF> ret;
	geos::geom::Polygon* pol = impl->m_polygon.get();

	const geos::geom::LineString* ls = pol->getExteriorRing();
	for (int i = 0; i < ls->getNumPoints() - 1; ++i) {
		const auto& c = ls->getCoordinateN(i);
		ret.push_back(QPointF(c.x, c.y));
	}
	for (int j = 0; j < pol->getNumInteriorRing(); ++j) {
		ls = pol->getInteriorRingN(j);
		for (int i = 0; i < ls->getNumPoints() - 1; ++i) {
			const auto& c = ls->getCoordinateN(i);
			ret.push_back(QPointF(c.x, c.y));
		}
	}

	return ret;
}

std::vector<unsigned int> GeoDataPolygonGroupPolygon::lineEdges() const
{
	std::vector<unsigned int> ret;
	geos::geom::Polygon* pol = impl->m_polygon.get();

	const geos::geom::LineString* ls = pol->getExteriorRing();

	unsigned int offset = 0;
	for (int i = 0; i < ls->getNumPoints() - 1; ++i) {
		ret.push_back(offset + i);
		ret.push_back(offset + (i + 1) % (ls->getNumPoints() - 1));
	}
	offset += (ls->getNumPoints() - 1);

	for (int j = 0; j < pol->getNumInteriorRing(); ++j) {
		ls = pol->getInteriorRingN(j);
		for (int i = 0; i < ls->getNumPoints() - 1; ++i) {
			ret.push_back(offset + i);
			ret.push_back(offset + (i + 1) % (ls->getNumPoints() - 1));
		}
		offset += (ls->getNumPoints() - 1);
	}
	return ret;
}

std::vector<unsigned int> GeoDataPolygonGroupPolygon::triangleCells() const
{
	return impl->m_triangleCells;
}

unsigned int GeoDataPolygonGroupPolygon::indexOffset() const
{
	return impl->m_indexOffset;
}

void GeoDataPolygonGroupPolygon::setIndexOffset(unsigned int offset)
{
	impl->m_indexOffset = offset;
}

void GeoDataPolygonGroupPolygon::getBoundingRect(double* xmin, double* xmax, double* ymin, double* ymax)
{
	*xmin = impl->m_xmin;
	*xmax = impl->m_xmax;
	*ymin = impl->m_ymin;
	*ymax = impl->m_ymax;
}

void GeoDataPolygonGroupPolygon::loadExternalData(QDataStream* stream)
{
	QString name;
	QVariant value;

	*stream >> name >> value;
	impl->m_name = name;
	impl->m_value = value;

	QVector<QPointF> points;
	*stream >> points;
	QPolygonF pol(points);
	geos::geom::LinearRing* elr = buildRing(pol);

	int numHoles;
	*stream >> numHoles;
	std::vector<geos::geom::Geometry*>* holes = new std::vector<geos::geom::Geometry*>();

	for (int i = 0; i < numHoles; ++i) {
		QVector<QPointF> points;
		*stream >> points;
		QPolygonF pol(points);
		geos::geom::LinearRing* ilr = buildRing(pol);
		holes->push_back(ilr);
	}
	auto f = geos::geom::GeometryFactory::getDefaultInstance();
	impl->m_polygon.reset(f->createPolygon(elr, holes));

	setupBoundingRect();
	setupTriangleCells();
}

void GeoDataPolygonGroupPolygon::saveExternalData(QDataStream* stream)
{
	*stream << impl->m_name;
	*stream << impl->m_value;
	geos::geom::Polygon* pol = impl->m_polygon.get();

	auto vec = buildPolygon(pol->getExteriorRing());
	*stream << vec;

	int numHoles = pol->getNumInteriorRing();
	*stream << numHoles;
	for (int i = 0; i < pol->getNumInteriorRing(); ++i) {
		auto vec = buildPolygon(pol->getInteriorRingN(i));
		*stream << vec;
	}
}

void GeoDataPolygonGroupPolygon::applyOffset(double x, double y)
{
	auto pol = impl->m_polygon.get();

	const auto els = pol->getExteriorRing();
	auto elr = ::applyOffset(els, x, y);

	auto holes = new std::vector<geos::geom::Geometry*>();
	for (int i = 0; i < pol->getNumInteriorRing(); ++i) {
		auto ils = pol->getInteriorRingN(i);
		auto ilr = ::applyOffset(ils, x, y);
		holes->push_back(ilr);
	}

	auto f = geos::geom::GeometryFactory::getDefaultInstance();
	impl->m_polygon.reset(f->createPolygon(elr, holes));
}

void GeoDataPolygonGroupPolygon::setupBoundingRect()
{
	const auto env = impl->m_polygon.get()->getEnvelopeInternal();

	impl->m_xmin = env->getMinX();
	impl->m_xmax = env->getMaxX();
	impl->m_ymin = env->getMinY();
	impl->m_ymax = env->getMaxY();
}

void GeoDataPolygonGroupPolygon::setupTriangleCells()
{
	TriangulatorI* triangulator = nullptr;
	auto pol = impl->m_polygon.get();
	if (pol->getNumInteriorRing() > 0) {
		triangulator = new TriangulatorTriangle();
	} else {
		triangulator = new TriangulatorVtk();
	}
	impl->m_triangleCells = triangulator->triangulate(this);

	delete triangulator;
}
