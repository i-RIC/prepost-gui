#include "pointstriangle.h"
#include "../../../misc/geometryutil.h"

#include <QPointF>
#include <QVector3D>

namespace {

bool crossedPoint(QVector3D* p1, QVector3D* p2, const QPointF& q1, const QPointF& q2, QVector3D* crossed)
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

PointsTriangle::PointsTriangle(QVector3D* p1, QVector3D* p2, QVector3D* p3)
{
	m_points[0] = p1;
	m_points[1] = p2;
	m_points[2] = p3;
}

QVector3D* PointsTriangle::point(int idx) const
{
	return m_points[idx % 3];
}

std::vector<QVector3D> PointsTriangle::crossedPoints(const QPointF& p1, const QPointF& p2) const
{
	std::vector<QVector3D> ret;
	QVector3D p;
	for (int i = 0; i < 3; ++i) {
		bool ok = crossedPoint(point(i), point(i + 1), p1, p2, &p);
		if (! ok) {continue;}
		ret.push_back(p);
	}
	return ret;
}
