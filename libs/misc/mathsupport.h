#ifndef MATHSUPPORT_H
#define MATHSUPPORT_H

#include "misc_global.h"

#include <Qt>

class QVector2D;
class QPointF;
class QPolygonF;

namespace iRIC
{
	qreal MISCDLL_EXPORT outerProduct(const QVector2D& v1, const QVector2D& v2);
	qreal MISCDLL_EXPORT angleRadian(const QVector2D& v1, const QVector2D& v2);
	qreal MISCDLL_EXPORT angle(const QVector2D& v1, const QVector2D& v2);
	qreal MISCDLL_EXPORT distance(const QPointF& p1, const QPointF& p2);
	void MISCDLL_EXPORT rotateVector90(QVector2D& v);
	void MISCDLL_EXPORT rotateVector180(QVector2D& v);
	void MISCDLL_EXPORT rotateVector270(QVector2D& v);
	void MISCDLL_EXPORT rotateVector(QVector2D& v, double angle);
	void MISCDLL_EXPORT rotateVectorRadian(QVector2D& v, double radianAngle);
	bool MISCDLL_EXPORT isInsideParallelogram(const QVector2D& target, const QVector2D& vertex, const QVector2D& dir1, const QVector2D& dir2);
	double MISCDLL_EXPORT roundedValue(double val, int precision);
	double MISCDLL_EXPORT pointRoundedValue(double val, int precision);
	double MISCDLL_EXPORT timeSecondValue(double val);
	const QString MISCDLL_EXPORT timeSecondValueStr(double val, int length = 0);
	bool MISCDLL_EXPORT hasIntersection(const QPolygonF& polygon);
}

#endif // MATHSUPPORT_H
