#include "rect.h"
#include "geometrypoint.h"
#include "geometrytriangle.h"
#include "../misc/geometryutil.h"

namespace {

bool crossedPoint(GeometryPoint* p1, GeometryPoint* p2, const QPointF& q1, const QPointF& q2, GeometryPoint* crossed)
{
	QPointF p1_2d(p1->x(), p1->y());
	QPointF p2_2d(p2->x(), p2->y());
	QPointF isec;
	double r, s;

	bool ok = GeometryUtil::intersectionPoint(p1_2d, p2_2d, q1, q2, &isec, &r, &s);
	if (ok == false) {return false;}
	if (r < 0 || r > 1) {return false;}
	if (s < 0 || s > 1) {return false;}

	double z = p1->z() * (1 - r) + p2->z() * r;
	crossed->setX(isec.x());
	crossed->setY(isec.y());
	crossed->setZ(z);

	return true;
}

} // namespace

GeometryTriangle::GeometryTriangle(GeometryPoint* p1, GeometryPoint* p2, GeometryPoint* p3)
{
	m_points[0] = p1;
	m_points[1] = p2;
	m_points[2] = p3;
}

GeometryPoint* GeometryTriangle::point(int idx) const
{
	return m_points[idx % 3];
}

std::vector<GeometryPoint> GeometryTriangle::crossedPoints(const QPointF& p1, const QPointF& p2) const
{
	std::vector<GeometryPoint> ret;
	GeometryPoint p;
	for (int i = 0; i < 3; ++i) {
		bool ok = crossedPoint(point(i), point(i + 1), p1, p2, &p);
		if (! ok) {continue;}
		ret.push_back(p);
	}
	return ret;
}

Rect GeometryTriangle::rect() const
{
	double xmin, xmax, ymin, ymax;
	for (int i = 0; i < 3; ++i) {
		GeometryPoint* p = m_points[i];
		if (i == 0 || p->x() < xmin) {xmin = p->x();}
		if (i == 0 || p->x() > xmax) {xmax = p->x();}
		if (i == 0 || p->y() < ymin) {ymin = p->y();}
		if (i == 0 || p->y() > ymax) {ymax = p->y();}
	}
	return Rect(xmin, ymin, xmax, ymax);
}
