#define _USE_MATH_DEFINES
#include <cmath>
#include "mathsupport.h"
#include <QPolygonF>
#include <QLineF>
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

	qreal outerProduct(const QPointF& v1, const QPointF& v2)
	{
		return v1.x() * v2.y() - v1.y() * v2.x();
	}

	qreal angleRadian(const QPointF& v1, const QPointF& v2)
	{
		qreal dotprod = QPointF::dotProduct(v1, v2);
		double cosVal = dotprod / (length(v1) * length(v2));
		if (cosVal <= -1) {return M_PI;}
		if (cosVal >= 1) {return 0;}
		qreal angle1 = acos(cosVal);
		QPointF tmp = v1;
		rotateVector90(tmp);
		if (QPointF::dotProduct(tmp, v2) < 0) {
			return 2 * M_PI - angle1;
		} else {
			return angle1;
		}
	}

	qreal distance(const QPointF& p1, const QPointF& p2)
	{
		QPointF v(p2.x() - p1.x(), p2.y() - p1.y());
		return length(v);
	}

	QPointF normalize(const QPointF& v)
	{
		return v / length(v);
	}

	qreal angle(const QPointF& v1, const QPointF& v2)
	{
		return angleRadian(v1, v2) / M_PI * 180.;
	}

	QPointF rotateVector90(QPointF& v)
	{
		double tmp = v.x();
		v.setX(- v.y());
		v.setY(tmp);

		return v;
	}

	QPointF rotateVector180(QPointF& v)
	{
		v.setX(- v.x());
		v.setY(- v.y());

		return v;
	}

	QPointF rotateVector270(QPointF& v)
	{
		double tmp = v.x();
		v.setX(v.y());
		v.setY(- tmp);

		return v;
	}

	QPointF rotateVector(QPointF& v, double angle)
	{
		double radianAngle = angle / 180. * M_PI;
		return rotateVectorRadian(v, radianAngle);
	}

	QPointF rotateVectorRadian(QPointF& v, double radianAngle)
	{
		double cosVal = std::cos(radianAngle);
		double sinVal = std::sin(radianAngle);
		double newX = cosVal * v.x() - sinVal * v.y();
		double newY = sinVal * v.x() + cosVal * v.y();
		v.setX(newX);
		v.setY(newY);

		return v;
	}

	bool isInsideParallelogram(const QPointF& target, const QPointF& vertex, const QPointF& dir1, const QPointF& dir2)
	{
		if (lengthSquared(dir1) < VERYSMALL) {return false;}
		if (lengthSquared(dir2) < VERYSMALL) {return false;}
		QPointF dv = target - vertex;
		QPointF v1 = normalize(dir1);
		QPointF v2 = normalize(dir2);
		double length1 = length(dir1);
		double length2 = length(dir2);
		double innerprod1 = QPointF::dotProduct(dv, v1);
		double innerprod2 = QPointF::dotProduct(dv, v2);
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

	double lengthSquared(const QPointF& p)
	{
		return p.x() * p.x() + p.y() * p.y();
	}

	double length(const QPointF& p)
	{
		return std::sqrt(lengthSquared(p));
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

	double perpendicularLineOfLeg(const QPointF& p1, const QPointF& p2, const QPointF& x, QPointF* leg)
	{
		double dx = p2.x() - p1.x();
		double dy = p2.y() - p1.y();
		double r = ((p2.x() - p1.x()) * (x.x() - p1.x()) + (p2.y() - p1.y()) * (x.y() - p1.y())) /
				(dx * dx + dy * dy);
		leg->setX((1 - r) * p1.x() + r * p2.x());
		leg->setY((1 - r) * p1.y() + r * p2.y());

		return r;
	}
}
