#include "elevationpoints.h"
#include "elevationpointspreprocessorview.h"

#include "private/elevationpoints_impl.h"
#include "private/pointstriangle.h"
#include "../../misc/geometryutil.h"

#define REAL double
#define VOID void
#include <triangle/triangle.h>

#include <QIcon>
#include <QStandardItem>
#include <QVector2D>
#include <QVector3D>

ElevationPoints::Impl::Impl(ElevationPoints *points) :
	m_parent {points}
{}

void ElevationPoints::Impl::buildTriangles()
{
	triangulateio in, out;

	auto points = m_parent->points();
	int pointCount = points.size();
	std::vector<double> pointList;
	pointList.reserve(pointCount);

	for (QVector3D* p : points) {
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
		QVector3D* p1 = points.at(*(out.trianglelist + i * 3    ) - 1);
		QVector3D* p2 = points.at(*(out.trianglelist + i * 3 + 1) - 1);
		QVector3D* p3 = points.at(*(out.trianglelist + i * 3 + 2) - 1);
		m_triangles.push_back(PointsTriangle(p1, p2, p3));
	}

	trifree(out.pointmarkerlist);
	trifree(out.trianglelist);
	trifree(out.segmentlist);
	trifree(out.segmentmarkerlist);
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

void ElevationPoints::setPoints(const std::vector<QVector3D*>& points)
{
	Points::setPoints(points);
	impl->buildTriangles();
}

std::vector<QVector2D> ElevationPoints::buildCrossSectionPoints(const QPointF& p1, const QPointF& p2) const
{
	std::vector<QVector2D> ret;

	for (const PointsTriangle& tri : impl->m_triangles) {
		auto points =  tri.crossedPoints(p1, p2);
		for (QVector3D p : points) {
			QVector2D diff(p.x() - p1.x(), p.y() - p1.y());
			double distance = diff.length();
			ret.push_back(QVector2D(distance, p.z()));
		}
	}
	return ret;
}
