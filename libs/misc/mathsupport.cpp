#define _USE_MATH_DEFINES
#include <cmath>
#include "mathsupport.h"
#include <QPolygonF>
#include <QLineF>
#include <QVector2D>
#include <QString>

// Precision of time
#define TIME_PRECISION 3
#define VERYSMALL 1.0E-20

namespace iRIC
{
	int nearRadius()
	{
		return 10;
	}

	bool isNear(const QPoint& p1, const QPoint& p2)
	{
		int r = nearRadius();

		int xDiff = qAbs(p2.x() - p1.x());
		int yDiff = qAbs(p2.y() - p1.y());
		return (xDiff < r && yDiff < r);
	}

	qreal outerProduct(const QVector2D& v1, const QVector2D& v2)
	{
		return v1.x() * v2.y() - v1.y() * v2.x();
	}

	qreal angleRadian(const QVector2D& v1, const QVector2D& v2)
	{
		qreal dotprod = QVector2D::dotProduct(v1, v2);
		double cosVal = dotprod / (v1.length() * v2.length());
		if (cosVal <= -1) {return M_PI;}
		if (cosVal >= 1) {return 0;}
		qreal angle1 = acos(cosVal);
		QVector2D tmp = v1;
		rotateVector90(tmp);
		if (QVector2D::dotProduct(tmp, v2) < 0) {
			return 2 * M_PI - angle1;
		} else {
			return angle1;
		}
	}

	qreal distance(const QPointF& p1, const QPointF& p2)
	{
		QVector2D v(p2.x() - p1.x(), p2.y() - p1.y());
		return v.length();
	}

	qreal angle(const QVector2D& v1, const QVector2D& v2)
	{
		return angleRadian(v1, v2) / M_PI * 180.;
	}

	void rotateVector90(QVector2D& v)
	{
		double tmp = v.x();
		v.setX(- v.y());
		v.setY(tmp);
	}

	void rotateVector180(QVector2D& v)
	{
		v.setX(- v.x());
		v.setY(- v.y());
	}

	void rotateVector270(QVector2D& v)
	{
		double tmp = v.x();
		v.setX(v.y());
		v.setY(- tmp);
	}

	void rotateVector(QVector2D& v, double angle)
	{
		double radianAngle = angle / 180. * M_PI;
		rotateVectorRadian(v, radianAngle);
	}

	void rotateVectorRadian(QVector2D& v, double radianAngle)
	{
		double cosVal = std::cos(radianAngle);
		double sinVal = std::sin(radianAngle);
		double newX = cosVal * v.x() - sinVal * v.y();
		double newY = sinVal * v.x() + cosVal * v.y();
		v.setX(newX);
		v.setY(newY);
	}

	bool isInsideParallelogram(const QVector2D& target, const QVector2D& vertex, const QVector2D& dir1, const QVector2D& dir2)
	{
		if (dir1.lengthSquared() < VERYSMALL) {return false;}
		if (dir2.lengthSquared() < VERYSMALL) {return false;}
		QVector2D dv = target - vertex;
		QVector2D v1 = dir1;
		v1.normalize();
		QVector2D v2 = dir2;
		v2.normalize();
		double length1 = dir1.length();
		double length2 = dir2.length();
		double innerprod1 = QVector2D::dotProduct(dv, v1);
		double innerprod2 = QVector2D::dotProduct(dv, v2);
		return (
						 0 <= innerprod1 && innerprod1 <= length1 &&
						 0 <= innerprod2 && innerprod2 <= length2
					 );
	}

	double roundedValue(double val, int precision)
	{
		if (val <= 0) {return val;}

		int e = 0;
		double tmpv = val;
		while (tmpv > 10) {
			tmpv = tmpv / 10;
			++ e;
		}
		while (tmpv < 1) {
			tmpv = tmpv * 10;
			-- e;
		}
		int p = precision - 1;
		for (int i = 0; i < p; ++i) {
			tmpv = tmpv * 10;
			-- e;
		}
		double ret = std::floor(tmpv + 0.5);
		while (e > 0) {
			ret = ret * 10;
			-- e;
		}
		while (e < 0) {
			ret = ret / 10;
			++ e;
		}
		return ret;
	}

	double pointRoundedValue(double val, int precision)
	{
		if (val <= 0) {return val;}
		double tmpv = val;
		for (int i = 0; i < precision; ++i) {
			tmpv = tmpv * 10;
		}
		tmpv = std::floor(tmpv + 0.5);
		for (int i = 0; i < precision; ++i) {
			tmpv = tmpv / 10;
		}
		return tmpv;
	}

	double timeSecondValue(double val)
	{
		return pointRoundedValue(val, TIME_PRECISION);
	}

	QString timeSecondValueStr(double val, int /*length*/)
	{
		QString str = QString::number(timeSecondValue(val), 'f', TIME_PRECISION);
		while (str.at(str.length() - 1) == '0') {
			str.chop(1);
		}
		if (str.at(str.length() - 1) == '.') {
			str.chop(1);
		}
		return str;
	}

	bool hasIntersection(const QPolygonF& polygon)
	{
		for (int i = 0; i < polygon.count() - 1; ++i) {
			for (int j = i + 1; j < polygon.count() - 1; ++j) {
				QLineF line1(polygon.at(i), polygon.at(i + 1));
				QLineF line2(polygon.at(j), polygon.at(j + 1));
				QPointF p;
				if (line1.intersect(line2, &p) == QLineF::BoundedIntersection) {
					if (p != line1.p1() && p != line1.p2()) {
						return true;
					}
				}
			}
		}
		return false;
	}

	bool intersectionPoint(const QPointF& p1, const QPointF& p2, const QPointF& q1, const QPointF& q2, QPointF* interSection, double* r, double* s)
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
}

