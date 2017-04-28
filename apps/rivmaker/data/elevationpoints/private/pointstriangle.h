#ifndef POINTSTRIANGLE_H
#define POINTSTRIANGLE_H

#include <vector>

class QPointF;
class QVector3D;

class PointsTriangle
{
public:
	PointsTriangle(QVector3D* p1, QVector3D* p2, QVector3D* p3);

	QVector3D* point(int idx) const;
	std::vector<QVector3D> crossedPoints(const QPointF& p1, const QPointF& p2) const;

private:
	QVector3D* m_points[3];
};

#endif // POINTSTRIANGLE_H
