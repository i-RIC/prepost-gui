#include "polygonutil.h"

#define REAL double

#include <vtkIdList.h>
#include <vtkPoints.h>
#include <vtkPolygon.h>
#include <vtkSmartPointer.h>

#include <triangle/triangle.h>

#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Point.h>
#include <geos/geom/Polygon.h>

#include <QPolygonF>

namespace {

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

} // namespace

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

void PolygonUtil::triangulate(const QPolygonF& polygon, std::vector<unsigned int>* indices)
{
	// triangulateTriangle(polygon, indices);
	triangulateVtk(polygon, indices);
}

void PolygonUtil::triangulateTriangle(const QPolygonF& polygon, std::vector<unsigned int>* indices)
{
	auto geosPolygon = getGeosPolygon(polygon);

	const geos::geom::LineString* eLS = geosPolygon->getExteriorRing();
	int pointCount = eLS->getNumPoints() - 1;
	std::vector<double> coords;
	coords.reserve(pointCount * 2);

	auto offset = eLS->getCoordinateN(0);
	for (int i = 0; i < pointCount; ++i) {
		auto coord = eLS->getCoordinateN(i);
		coords.push_back(coord.x - offset.x);
		coords.push_back(coord.y - offset.y);
	}
	std::vector<int> segs;
	segs.reserve(pointCount * 2);
	for (int i = 0; i < pointCount; ++i) {
		segs.push_back(i + 1);
		segs.push_back((i + 1) % pointCount + 1);
	}
	std::vector<double> region;
	region.reserve(4);
	auto internalPoint = geosPolygon->getInteriorPoint();
	region.push_back(internalPoint->getX() - offset.x);
	region.push_back(internalPoint->getY() - offset.y);
	region.push_back(0);
	region.push_back(geosPolygon->getEnvelope()->getArea());
	delete internalPoint;

	triangulateio in;
	clearTrianglateio(&in);
	in.pointlist = coords.data();
	in.numberofpoints = pointCount;
	in.segmentlist = segs.data();
	in.numberofsegments = pointCount;
	in.numberofholes = 0;
	in.regionlist = region.data();
	in.numberofregions = 1;

	triangulateio out;
	clearTrianglateio(&out);
	char arg[] = "pQ";
	::triangulate(&(arg[0]), &in, &out, nullptr);

	int triId = 0;
	indices->reserve(out.numberoftriangles * 3);
	while (triId < out.numberoftriangles) {
		indices->push_back(*(out.trianglelist + triId * 3) - 1);
		indices->push_back(*(out.trianglelist + triId * 3 + 1) - 1);
		indices->push_back(*(out.trianglelist + triId * 3 + 2) - 1);
		++triId;
	}

	trifree(out.pointlist);
	trifree(out.trianglelist);

	delete geosPolygon;
}

void PolygonUtil::triangulateVtk(const QPolygonF& polygon, std::vector<unsigned int>* indices)
{
	auto pol = vtkSmartPointer<vtkPolygon>::New();
	auto points = pol->GetPoints();
	auto ids = pol->GetPointIds();

	auto firstP = polygon.at(0);
	for (int i = 0; i < polygon.size() - 1; ++i) {
		auto p = polygon.at(i);
		points->InsertNextPoint(p.x() - firstP.x(), p.y() - firstP.y(), 0);
		ids->InsertNextId(i);
	}
	auto triIds = vtkSmartPointer<vtkIdList>::New();
	pol->Triangulate(triIds);

	indices->clear();
	for (int i = 0; i < triIds->GetNumberOfIds(); ++i) {
		indices->push_back(triIds->GetId(i));
	}
}

PolygonUtil::PolygonUtil()
{}
