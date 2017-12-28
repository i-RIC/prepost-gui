#ifndef GEOMETRYUTIL_H
#define GEOMETRYUTIL_H

class QPointF;

class GeometryUtil
{
public:
	static bool intersectionPoint(const QPointF& p1, const QPointF& p2, const QPointF& q1, const QPointF& q2, QPointF* interSection, double* r, double* s);
	static QPointF nearestPoint(const QPointF& a, const QPointF& b, const QPointF& p, int* posType = nullptr);
	static QPointF mappedPoint(const QPointF& a, const QPointF& b, const QPointF& p);

private:
	GeometryUtil();
};

#endif // GEOMETRYUTIL_H
