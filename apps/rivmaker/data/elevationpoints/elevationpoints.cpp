#include "elevationpoints.h"
#include "elevationpointspreprocessorview.h"

#include "private/elevationpoints_impl.h"
#include "../../geom/geometrypoint.h"
#include "../../geom/geometrytriangle.h"
#include "../../geom/rect.h"
#include "../../misc/geometryutil.h"

#define REAL double
#define VOID void
#include <triangle/triangle.h>

#include <QIcon>
#include <QStandardItem>
#include <QVector2D>

ElevationPoints::Impl::Impl(ElevationPoints *points) :
	m_parent {points}
{}

void ElevationPoints::Impl::buildTriangles()
{
	if (m_parent->points().size() < 3) {return;}

	triangulateio in, out;

	auto points = m_parent->points();
	int pointCount = static_cast<int>(points.size());
	std::vector<double> pointList;
	pointList.reserve(pointCount);

	for (GeometryPoint* p : points) {
		pointList.push_back(p->x());
		pointList.push_back(p->y());
	}

	in.pointlist = pointList.data();
	in.pointattributelist = NULL;
	in.pointmarkerlist = NULL;
	in.numberofpoints = pointCount;
	in.numberofpointattributes = 0;

	in.trianglelist = NULL;
	in.triangleattributelist = NULL;
	in.trianglearealist = NULL;
	in.neighborlist = NULL;
	in.numberoftriangles = 0;
	in.numberofcorners = 0;
	in.numberoftriangleattributes = 0;

	in.segmentlist = NULL;
	in.segmentmarkerlist = NULL;
	in.numberofsegments = 0;

	in.holelist = NULL;
	in.numberofholes = 0;

	in.regionlist = NULL;
	in.numberofregions = 0;

	in.edgelist = NULL;
	in.edgemarkerlist = NULL;
	in.normlist = NULL;
	in.numberofedges = 0;

	out.pointlist = in.pointlist;
	out.pointattributelist = NULL;
	out.pointmarkerlist = NULL;
	out.trianglelist = NULL;
	out.triangleattributelist = NULL;
	out.trianglearealist = NULL;
	out.neighborlist = NULL;
	out.segmentlist = NULL;
	out.segmentmarkerlist = NULL;
	out.holelist = NULL;
	out.regionlist = NULL;
	out.edgelist = NULL;
	out.edgemarkerlist = NULL;
	out.normlist = NULL;

	char arg[] = "pcj";

	triangulate(arg, &in, &out, 0);

	m_triangles.clear();
	for (int i = 0; i < out.numberoftriangles; ++i) {
		GeometryPoint* p1 = points.at(*(out.trianglelist + i * 3    ) - 1);
		GeometryPoint* p2 = points.at(*(out.trianglelist + i * 3 + 1) - 1);
		GeometryPoint* p3 = points.at(*(out.trianglelist + i * 3 + 2) - 1);
		m_triangles.push_back(GeometryTriangle(p1, p2, p3));
	}

	trifree(out.pointmarkerlist);
	trifree(out.trianglelist);
	trifree(out.segmentlist);
	trifree(out.segmentmarkerlist);
}

void ElevationPoints::Impl::buildContainers()
{
	m_pointsContainer.clear();

	for (GeometryPoint* p : m_parent->points()) {
		m_pointsContainer.add(p);
	}

	m_trianglesContainer.clear();
	for (GeometryTriangle& t : m_triangles) {
		m_trianglesContainer.add(&t);
	}
}

// public interfaces

ElevationPoints::ElevationPoints(DataItem *parent) :
	Points {parent},
	impl {new Impl {this}}
{}

ElevationPoints::~ElevationPoints()
{
	delete impl;
}

QString ElevationPoints::caption() const
{
	return tr("Elevation Points");
}

QStandardItem* ElevationPoints::buildPreProcessorStandardItem() const
{
	auto item = new QStandardItem(QIcon(":/images/iconElevationPoints.png"), tr("Elevation Points"));
	setupStandardItem(item);
	return item;
}

DataItemView* ElevationPoints::buildPreProcessorDataItemView(Model* model)
{
	return new ElevationPointsPreprocessorView(model, this);
}

void ElevationPoints::setPoints(const std::vector<GeometryPoint*> &points)
{
	Points::setPoints(points);
	impl->buildTriangles();
	impl->buildContainers();
}


std::vector<GeometryPoint*> ElevationPoints::pointsInRect(double xmin, double ymin, double xmax, double ymax) const
{
	Rect rect(xmin, ymin, xmax, ymax);
	auto pret = impl->m_pointsContainer.findIntersectRect(rect);

	std::vector<GeometryPoint*> ret;
	for (Geometry* g : pret) {
		auto p = dynamic_cast<GeometryPoint*> (g);
		ret.push_back(p);
	}
	return ret;
}

std::vector<QVector2D> ElevationPoints::buildCrossSectionPoints(const QPointF& p1, const QPointF& p2) const
{
	std::vector<QVector2D> ret;

	Rect rect(p1.x(), p1.y(), p2.x(), p2.y());
	auto tris = impl->m_trianglesContainer.findIntersectRect(rect);

	for (Geometry* g : tris) {
		auto tri = dynamic_cast<GeometryTriangle*> (g);
		auto points =  tri->crossedPoints(p1, p2);
		for (GeometryPoint p : points) {
			QVector2D diff(p.x() - p1.x(), p.y() - p1.y());
			double distance = diff.length();
			ret.push_back(QVector2D(distance, p.z()));
		}
	}
	return ret;
}

void ElevationPoints::loadExternalData(const QString& filename)
{
	Points::loadExternalData(filename);
	impl->buildTriangles();
	impl->buildContainers();
}

QString ElevationPoints::relativeFilename() const
{
	return "elevationpoints.dat";
}
