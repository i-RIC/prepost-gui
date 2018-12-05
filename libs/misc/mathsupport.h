#ifndef MATHSUPPORT_H
#define MATHSUPPORT_H

#include "misc_global.h"

class QVector2D;
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
	qreal MISCDLL_EXPORT outerProduct(const QVector2D& v1, const QVector2D& v2);
	/// The angle between two vectors (in radians)
	qreal MISCDLL_EXPORT angleRadian(const QVector2D& v1, const QVector2D& v2);
	/// The angle between two vectors (in degrees)
	qreal MISCDLL_EXPORT angle(const QVector2D& v1, const QVector2D& v2);
	/// The distance between two points
	qreal MISCDLL_EXPORT distance(const QPointF& p1, const QPointF& p2);
	/// Rotate the two-dimensional vector 90 degrees against anti-clockwise direction
	void MISCDLL_EXPORT rotateVector90(QVector2D& v);
	/// Rotate the two-dimensional vector 180 degrees against anti-clockwise direction
	void MISCDLL_EXPORT rotateVector180(QVector2D& v);
	/// Rotate the two-dimensional vector 270 degrees against anti-clockwise direction
	void MISCDLL_EXPORT rotateVector270(QVector2D& v);
	/// Rotate the two-dimensional vector against anti-clockwise direction with specified angle (in degrees)
	void MISCDLL_EXPORT rotateVector(QVector2D& v, double angle);
	/// Rotate the two-dimensional vector against anti-clockwise direction with specified angle (in radians)
	void MISCDLL_EXPORT rotateVectorRadian(QVector2D& v, double radianAngle);
	bool MISCDLL_EXPORT isInsideParallelogram(const QVector2D& target, const QVector2D& vertex, const QVector2D& dir1, const QVector2D& dir2);
	/// Returns rounded value
	double MISCDLL_EXPORT roundedValue(double val, int precision);
	double MISCDLL_EXPORT pointRoundedValue(double val, int precision);
	/// Returns rounded value appropriate for time seconds (until miliseconds)
	double MISCDLL_EXPORT timeSecondValue(double val);
	/// Returns timeSecondValue() as a string
	QString MISCDLL_EXPORT timeSecondValueStr(double val, int length = 0);
	/// Returns true if polygon edges cross with other edge of the polygon
	bool MISCDLL_EXPORT hasIntersection(const QPolygonF& polygon);
	/// Returns intersection point of two lines
	bool MISCDLL_EXPORT intersectionPoint(const QPointF& p1, const QPointF& p2, const QPointF& q1, const QPointF& q2, QPointF* interSection, double* r, double* s);
}

#endif // MATHSUPPORT_H
