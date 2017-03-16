#ifndef GEOMETRYUTIL_H
#define GEOMETRYUTIL_H

class QPointF;

class GeometryUtil
{
public:
	static bool intersectionPoint(const QPointF& p1, const QPointF& p2, const QPointF& q1, const QPointF& q2, QPointF* interSection, double* r, double* s);

private:
	GeometryUtil();
};

#endif // GEOMETRYUTIL_H
