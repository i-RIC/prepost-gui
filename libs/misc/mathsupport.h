#ifndef MATHSUPPORT_H
#define MATHSUPPORT_H

#include "misc_global.h"

class QPoint;
class QPointF;
class QPolygonF;

namespace iRIC
{
	/// The radius used to decide whether it is near or not
	int MISCDLL_EXPORT nearRadius();
	/// Returns true if the distance is smaller than nearRadius()
	bool MISCDLL_EXPORT isNear(const QPoint& p1, const QPoint& p2);
	/// The outer product of two-dimensional vectors
	qreal MISCDLL_EXPORT outerProduct(const QPointF& v1, const QPointF& v2);
	/// The angle between two vectors (in radians)
	qreal MISCDLL_EXPORT angleRadian(const QPointF& v1, const QPointF& v2);
	/// The angle between two vectors (in degrees)
	qreal MISCDLL_EXPORT angle(const QPointF& v1, const QPointF& v2);
	/// The distance between two points
	qreal MISCDLL_EXPORT distance(const QPointF& p1, const QPointF& p2);
	/// Normalize the vector
	QPointF MISCDLL_EXPORT normalize(const QPointF& v);
	/// Rotate the two-dimensional vector 90 degrees against anti-clockwise direction
	QPointF MISCDLL_EXPORT rotateVector90(const QPointF& v);
	/// Rotate the two-dimensional vector 180 degrees against anti-clockwise direction
	QPointF MISCDLL_EXPORT rotateVector180(const QPointF& v);
	/// Rotate the two-dimensional vector 270 degrees against anti-clockwise direction
	QPointF MISCDLL_EXPORT rotateVector270(const QPointF& v);
	/// Rotate the two-dimensional vector against anti-clockwise direction with specified angle (in degrees)
	QPointF MISCDLL_EXPORT rotateVector(const QPointF& v, double angle);
	/// Rotate the two-dimensional vector against anti-clockwise direction with specified angle (in radians)
	QPointF MISCDLL_EXPORT rotateVectorRadian(const QPointF& v, double radianAngle);
	bool MISCDLL_EXPORT isInsideParallelogram(const QPointF& target, const QPointF& vertex, const QPointF& dir1, const QPointF& dir2);
	/// Returns rounded value
	double MISCDLL_EXPORT roundedValue(double val, int precision);
	double MISCDLL_EXPORT pointRoundedValue(double val, int precision);
	/// Returns rounded value appropriate for time seconds (until miliseconds)
	double MISCDLL_EXPORT timeSecondValue(double val);
	/// Returns timeSecondValue() as a string
	QString MISCDLL_EXPORT timeSecondValueStr(double val, int length = 0);
	/// Returns true if polygon edges cross with other edge of the polygon
	bool MISCDLL_EXPORT hasIntersection(const QPolygonF& polygon);
	/// Returns the length of QPointF
	double MISCDLL_EXPORT lengthSquared(const QPointF& p);
	/// Returns the length of QPointF
	double MISCDLL_EXPORT length(const QPointF& p);
	/// Returns intersection point of two lines
	bool MISCDLL_EXPORT intersectionPoint(const QPointF& p1, const QPointF& p2, const QPointF& q1, const QPointF& q2, QPointF* interSection, double* r, double* s);
	/// Return Perpendicular line of the leg
	double MISCDLL_EXPORT perpendicularLineOfLeg(const QPointF& p1, const QPointF& p2, const QPointF& x, QPointF* leg);
}

#endif // MATHSUPPORT_H
