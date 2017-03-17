#include "geometryutil.h"

#include <QPointF>

bool GeometryUtil::intersectionPoint(const QPointF& p1, const QPointF& p2, const QPointF& q1, const QPointF& q2, QPointF* interSection, double* r, double* s)
{
	double M =
			(p2.x() - p1.x()) * (q2.y() - q1.y()) -
			(p2.y() - p1.y()) * (q2.x() - q1.x());
	if (M == 0) {return false;}

	QPointF p1q1 = q1 - p1;

	*r = ((q2.y() - q1.y()) * p1q1.x() - (q2.x() - q1.x()) * p1q1.y()) / M;
	*s = ((p2.y() - p1.y()) * p1q1.x() - (p2.x() - p1.x()) * p1q1.y()) / M;

	*interSection = p1 + *r * (p2 - p1);

	return true;
}
