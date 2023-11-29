#define REAL double
#define VOID void

#include "geodatapolygongrouppolygon_impl.h"
#include "geodatapolygongrouppolygon_triangulatortriangle.h"

#include <triangle/triangle.h>

#include <geos/geom/GeometryFactory.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Point.h>
#include <geos/geom/Polygon.h>

#include <QPointF>

#include <map>

namespace {

struct Point2D {
	Point2D(double xx, double yy):
		x {xx},
		y {yy}
	{}
	bool operator<(const Point2D& p) const {
		if (x != p.x) {return x < p.x;}

		return y < p.y;
	}

	double x;
	double y;
};

template <typename T>
void freeMemory(T* memory)
{
	if (memory == NULL) {return;}
	trifree(memory);
}

void freeTriangleInput(triangulateio* in)
{
	delete in->pointlist;
	delete in->segmentlist;
	delete in->regionlist;
	delete in->holelist;
}

void freeTriangleOutput(triangulateio* out)
{
	freeMemory(out->pointlist);
	freeMemory(out->trianglelist);
}

void clearTrianglateio(triangulateio* io)
{
	io->pointlist = NULL;
	io->pointattributelist = NULL;
	io->pointmarkerlist = NULL;
	io->numberofpoints = 0;
	io->numberofpointattributes = 0;
	io->trianglelist = NULL;
	io->triangleattributelist = NULL;
	io->trianglearealist = NULL;
	io->neighborlist = NULL;
	io->numberoftriangles = 0;
	io->numberofcorners = 0;
	io->numberoftriangleattributes = 0;
	io->segmentlist = NULL;
	io->segmentmarkerlist = NULL;
	io->numberofsegments = 0;
	io->holelist = NULL;
	io->numberofholes = 0;
	io->regionlist = NULL;
	io->numberofregions = 0;
	io->edgelist = NULL;
	io->edgemarkerlist = NULL;
	io->normlist = NULL;
	io->numberofedges = 0;
}

void addPointsAndSegments(triangulateio* in, std::map<Point2D, unsigned int>* pointMap, std::vector<int>* pointIdVec, int* pointCount, int* pOff, int* sOff, const QPointF& cOff, const geos::geom::LineString* ls)
{
	std::vector<int> pointIds;

	for (int i = 0; i < static_cast<int> (ls->getNumPoints()) - 1; ++i) {
		const auto& coord = ls->getCoordinateN(i);
		Point2D point(coord.x - cOff.x(), coord.y - cOff.y());
		auto it = pointMap->find(point);
		unsigned int pointId;
		if (it == pointMap->end()) {
			pointId = static_cast<int> (pointMap->size());
			pointMap->insert({point, pointId});
			pointIdVec->push_back(*pointCount);
			*(in->pointlist + *pOff)     = point.x;
			*(in->pointlist + *pOff + 1) = point.y;
			*pOff += 2;
		} else {
			pointId = it->second;
		}
		pointIds.push_back(pointId);
		*pointCount += 1;
	}

	std::vector<int> segmentVec;

	for (int i = 0; i < static_cast<int> (pointIds.size()); ++i) {
		*(in->segmentlist + *sOff)     = pointIds.at(i) + 1;
		*(in->segmentlist + *sOff + 1) = pointIds.at((i + 1) % pointIds.size()) + 1;
		segmentVec.push_back(pointIds.at(i) + 1);
		segmentVec.push_back(pointIds.at((i + 1) % pointIds.size()) + 1);

		*sOff += 2;
	}
}

} // namespace

std::vector<unsigned int> GeoDataPolygonGroupPolygon::TriangulatorTriangle::triangulate(GeoDataPolygonGroupPolygon* polygon)
{
	int pointCount = 0;
	int segmentCount = 0;
	int regionCount = 1;

	geos::geom::Polygon* pol = polygon->impl->m_polygon.get();
	const geos::geom::LineString* eLS = pol->getExteriorRing();
	pointCount += (static_cast<int> (eLS->getNumPoints()) - 1);
	segmentCount += (static_cast<int> (eLS->getNumPoints()) - 1);
	for (int i = 0; i < pol->getNumInteriorRing(); ++i) {
		const geos::geom::LineString* iLS = pol->getInteriorRingN(i);
		pointCount += (static_cast<int> (iLS->getNumPoints()) - 1);
		segmentCount += (static_cast<int> (iLS->getNumPoints()) - 1);
	}
	triangulateio in;
	clearTrianglateio(&in);

	in.pointlist = new double[pointCount * 2];
	in.numberofpoints = pointCount;
	in.segmentlist = new int[segmentCount * 2];
	in.numberofsegments = segmentCount;
	in.holelist = new double[pol->getNumInteriorRing() * 2];
	in.numberofholes = static_cast<int> (pol->getNumInteriorRing());
	in.regionlist = new double[regionCount * 4];
	in.numberofregions = regionCount;

	const auto& coord = eLS->getCoordinateN(0);
	QPointF offset(coord.x, coord.y);

	// setup pointlist and segmentlist
	pointCount = 0;
	int pOff = 0;
	int sOff = 0;
	std::map<Point2D, unsigned int> pointMap;
	std::vector<int> pointIdVec;

	addPointsAndSegments(&in, &pointMap, &pointIdVec, &pointCount, &pOff, &sOff, offset, eLS);
	for (int i = 0; i < pol->getNumInteriorRing(); ++i) {
		const geos::geom::LineString* iLS = pol->getInteriorRingN(i);
		addPointsAndSegments(&in, &pointMap, &pointIdVec, &pointCount, &pOff, &sOff, offset, iLS);
	}
	in.numberofpoints = static_cast<int> (pointIdVec.size());

	// setup regionlist
	geos::geom::Point* ip = nullptr;
	try {
		ip = pol->getInteriorPoint();
		*(in.regionlist + 0) = ip->getX() - offset.x();
		*(in.regionlist + 1) = ip->getY() - offset.y();
		*(in.regionlist + 2) = 0;
		*(in.regionlist + 3) = pol->getEnvelope()->getArea();
		delete ip;
	} catch (...) {
		std::vector<unsigned int> ret;
		return ret;
	}

	// setup holelist
	auto f = geos::geom::GeometryFactory::getDefaultInstance();
	for (int i = 0; i < pol->getNumInteriorRing(); ++i) {
		const geos::geom::LineString* iLS = pol->getInteriorRingN(i);
		geos::geom::LinearRing* lr = f->createLinearRing(*(iLS->getCoordinatesRO()));
		std::vector<geos::geom::Geometry*>* emptyHoles = new std::vector<geos::geom::Geometry*>();
		geos::geom::Polygon* tmpPoly = f->createPolygon(lr, emptyHoles);
		ip = tmpPoly->getInteriorPoint();
		*(in.holelist + i * 2 + 0) = ip->getX() - offset.x();
		*(in.holelist + i * 2 + 1) = ip->getY() - offset.y();

		delete ip;
		delete tmpPoly;
	}
	triangulateio out;
	clearTrianglateio(&out);
	char arg[] = "pQ";
	::triangulate(&(arg[0]), &in, &out, nullptr);
	// freeTriangleInput(&in);

	std::vector<unsigned int> ret;
	ret.reserve(out.numberoftriangles * 3);
	for (int i = 0; i < out.numberoftriangles; ++i) {
		for (int j = 0; j < 3; ++j) {
			int pointId = *(out.trianglelist + i * 3 + j) - 1;
			ret.push_back(pointIdVec.at(pointId));
			// ret.push_back(pointId);
		}
	}
	freeTriangleOutput(&out);
	return ret;
}
