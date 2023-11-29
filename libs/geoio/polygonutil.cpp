#include "polygonutil.h"

#define REAL double

#include <vtkIdList.h>
#include <vtkPoints.h>
#include <vtkPolygon.h>
#include <vtkSmartPointer.h>

#include <triangle/triangle.h>
#include <triangle/triangleutil.h>

#include <geos/geom/CoordinateSequence.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/LinearRing.h>
#include <geos/geom/LineString.h>
#include <geos/geom/Point.h>
#include <geos/geom/Polygon.h>

#include <QPolygonF>

namespace {

template <typename T>
void freeMemory(T* memory)
{
	if (memory == NULL) {return;}
	trifree(memory);
}

void freeTriangleOutput(triangulateio* out)
{
	freeMemory(out->pointlist);
	freeMemory(out->trianglelist);
}

} // namespace

bool operator<(const QPointF& p1, const QPointF& p2)
{
	if (p1.x() != p2.x()) {
		return p1.x() < p2.x();
	} else {
		return p1.y() < p2.y();
	}
}

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
	std::vector<QPolygonF> emptyHoles;
	return getGeosPolygon(polygon, emptyHoles);
}

geos::geom::Polygon* PolygonUtil::getGeosPolygon(const QPolygonF& region, const std::vector<QPolygonF>& holes)
{
	auto shell = getLinearRing(region);

	auto holes2 = new std::vector<geos::geom::Geometry*>();
	for (const auto& h : holes) {
		holes2->push_back(getLinearRing(h));
	}

	auto f = geos::geom::GeometryFactory::getDefaultInstance();
	return f->createPolygon(shell, holes2);
}

geos::geom::LinearRing* PolygonUtil::getLinearRing(const QPolygonF& polygon)
{
	auto f = geos::geom::GeometryFactory::getDefaultInstance();
	const auto csf = f->getCoordinateSequenceFactory();
	auto cs = csf->create(polygon.size(), 2);
	for (int i = 0; i < polygon.size(); ++i) {
		auto p = polygon.at(i);
		geos::geom::Coordinate c(p.x(), p.y());
		cs->setAt(c, i);
	}

	return f->createLinearRing(cs);
}

void PolygonUtil::triangulate(const QPolygonF& polygon, std::vector<unsigned int>* indices)
{
	triangulateVtk(polygon, indices);
}

void PolygonUtil::triangulate(const QPolygonF& region, const std::vector<QPolygonF>& holes, std::vector<QPointF>* outPoints, std::vector<unsigned int>* indices)
{
	auto polygon = getGeosPolygon(region, holes);

	triangulate(polygon, outPoints, indices);

	delete polygon;
}

void PolygonUtil::triangulate(geos::geom::Polygon* polygon, std::vector<QPointF>* points, std::vector<unsigned int>* indices)
{
	if (polygon->getNumInteriorRing() == 0) {
		return triangulateVtk(polygon, points, indices);
	} else {
		return triangulateTriangle(polygon, points, indices);
	}
}

void PolygonUtil::triangulateTriangle(geos::geom::Polygon* polygon, std::vector<QPointF>* points, std::vector<unsigned int>* indices)
{
	auto f = geos::geom::GeometryFactory::getDefaultInstance();

	// check conditions

	// shell has enough points
	auto shell = dynamic_cast<const geos::geom::LinearRing*> (polygon->getExteriorRing());
	if (shell->getNumPoints() < 4) {return;}

	// rings are valid
	if (! shell->isValid()) {return;}
	for (unsigned int i = 0; i < polygon->getNumInteriorRing(); ++i) {
		auto hole = polygon->getInteriorRingN(i);
		if (! hole->isValid()) {return;}
	}

	std::vector<geos::geom::Geometry*> emptyHoles;
	auto shellPolygon = f->createPolygon(*shell, emptyHoles);

	std::vector<geos::geom::Polygon*> holePolygons;
	for (unsigned int i = 0; i < polygon->getNumInteriorRing(); ++i) {
		auto hole = dynamic_cast<const geos::geom::LinearRing*> (polygon->getInteriorRingN(i));
		auto holePolygon = f->createPolygon(*hole, emptyHoles);
		holePolygons.push_back(holePolygon);
	}

	// shell contains all holes
	for (auto hole : holePolygons) {
		if (! shellPolygon->contains(hole)) {return;}
	}

	// holes does not cross eacn other
	for (unsigned int i = 0; i < holePolygons.size(); ++i) {
		auto hole1 = holePolygons.at(i);
		for (unsigned int j = i + 1; j < holePolygons.size(); ++j) {
			auto hole2 = holePolygons.at(j);
			if (hole1->intersects(hole2)) {return;}
		}
	}

	delete shellPolygon;
	for (auto hole : holePolygons) {
		delete hole;
	}

	std::vector<double> pointList;
	std::vector<int> segmentList;
	std::vector<double> holeList;
	std::map<QPointF, unsigned int> pointMap;
	std::vector<double> regionList;
	regionList.assign(4, 0);

	auto offsetP = shell->getCoordinateN(0);
	QPointF offset(offsetP.x, offsetP.y);
	triangleAddPointsAndSegments(shell, &pointList, &segmentList, &pointMap, offset);
	for (unsigned int i = 0; i < polygon->getNumInteriorRing(); ++i) {
		auto hole = dynamic_cast<const geos::geom::LinearRing*>(polygon->getInteriorRingN(i));
		triangleAddPointsAndSegments(hole, &pointList, &segmentList, &pointMap, offset);
		triangleAddHole(hole, &holeList);
	}
	auto innerP = polygon->getInteriorPoint();
	regionList[0] = innerP->getX();
	regionList[1] = innerP->getY();
	regionList[2] = 0;
	regionList[3] = polygon->getEnvelope()->getArea();

	triangulateio in, out;

	TriangleUtil::clearTriangulateio(&in);
	TriangleUtil::clearTriangulateio(&out);

	in.numberofpoints = static_cast<int> (pointList.size()) / 2;
	in.pointlist = pointList.data();
	in.numberofsegments = static_cast<int> (segmentList.size()) / 2;
	in.segmentlist = segmentList.data();
	in.numberofholes = static_cast<int> (holeList.size()) / 2;
	in.holelist = holeList.data();
	in.numberofregions = 1;
	in.regionlist = regionList.data();

	char arg[] = "pQ";
	::triangulate(arg, &in, &out, 0);

	for (int i = 0; i < static_cast<int> (pointList.size()) / 2; ++i) {
		QPointF point(pointList.at(i * 2), pointList.at(i * 2 + 1));
		points->push_back(point);
	}

	for (int i = 0; i < out.numberoftriangles * 3; ++i) {
		indices->push_back(*(out.trianglelist + i) - 1);
	}

	freeTriangleOutput(&out);
}

void PolygonUtil::triangulateVtk(geos::geom::Polygon* polygon, std::vector<QPointF>* outPoints, std::vector<unsigned int>* indices)
{
	auto pol = vtkSmartPointer<vtkPolygon>::New();
	auto triIds = vtkSmartPointer<vtkIdList>::New();

	auto points = pol->GetPoints();
	auto ids = pol->GetPointIds();
	auto shell = polygon->getExteriorRing();

	if (shell->getNumPoints() < 4) {return;}
	auto first = shell->getCoordinateN(0);
	outPoints->reserve(shell->getNumPoints());
	for (int i = 0; i < static_cast<int> (shell->getNumPoints()) - 1; ++i) {
		auto p = shell->getCoordinateN(i);
		points->InsertNextPoint(p.x - first.x, p.y - first.y, 0);
		ids->InsertNextId(i);

		outPoints->push_back(QPointF(p.x, p.y));
	}
	pol->Triangulate(triIds);

	indices->resize(0);
	indices->reserve(triIds->GetNumberOfIds());
	for (int i = 0; i < triIds->GetNumberOfIds(); ++i) {
		indices->push_back(triIds->GetId(i));
	}
}

void PolygonUtil::triangleAddPointsAndSegments(const geos::geom::LinearRing* ring, std::vector<double>* pointList, std::vector<int>* segmentList, std::map<QPointF, unsigned int>* pointMap, const QPointF& offset)
{
	std::vector<int> pointIds;

	for (int i = 0; i < static_cast<int> (ring->getNumPoints()) - 1; ++i) {
		const auto& coord = ring->getCoordinateN(i);
		QPointF point(coord.x - offset.x(), coord.y - offset.y());
		auto it = pointMap->find(point);
		unsigned int pointId;
		if (it == pointMap->end()) {
			pointId = static_cast<unsigned int> (pointMap->size());
			pointMap->insert({point, pointId});
			pointList->push_back(coord.x);
			pointList->push_back(coord.y);
		} else {
			pointId = it->second;
		}
		pointIds.push_back(pointId);
	}

	for (int i = 0; i < static_cast<int> (pointIds.size()); ++i) {
		segmentList->push_back(pointIds.at(i) + 1);
		segmentList->push_back(pointIds.at((i + 1) % pointIds.size()) + 1);
	}
}

void PolygonUtil::triangleAddHole(const geos::geom::LinearRing* ring, std::vector<double>* holeList)
{
	std::vector<geos::geom::Geometry*> holes;
	auto f = geos::geom::GeometryFactory::getDefaultInstance();
	auto polygon = f->createPolygon(*ring, holes);

	auto point = polygon->getInteriorPoint();
	holeList->push_back(point->getX());
	holeList->push_back(point->getY());
}

void PolygonUtil::triangulateVtk(const QPolygonF& polygon, std::vector<unsigned int>* indices)
{
	auto pol = vtkSmartPointer<vtkPolygon>::New();
	auto triIds = vtkSmartPointer<vtkIdList>::New();
	triangulateVtk(polygon, indices, pol, triIds);
}

void PolygonUtil::triangulateVtk(const QPolygonF& polygon, std::vector<unsigned int>* indices, vtkPolygon* pol, vtkIdList* triIds)
{
	auto points = pol->GetPoints();
	auto ids = pol->GetPointIds();
	points->Reset();
	ids->Reset();
	triIds->Reset();

	auto firstP = polygon.at(0);
	for (int i = 0; i < polygon.size() - 1; ++i) {
		auto p = polygon.at(i);
		points->InsertNextPoint(p.x() - firstP.x(), p.y() - firstP.y(), 0);
		ids->InsertNextId(i);
	}
	pol->Triangulate(triIds);

	indices->resize(0);
	indices->reserve(triIds->GetNumberOfIds());
	for (int i = 0; i < triIds->GetNumberOfIds(); ++i) {
		indices->push_back(triIds->GetId(i));
	}
}

PolygonUtil::PolygonUtil()
{}
