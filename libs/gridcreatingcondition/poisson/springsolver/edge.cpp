#include "edge.h"

#include <QPointF>
#include <QVector2D>

Edge::Edge()
{}

void Edge::setLine(const std::vector<QPointF>& line)
{
	std::vector<double> x, y;
	for (const QPointF& p : line) {
		x.push_back(p.x());
		y.push_back(p.y());
	}
	setXY(x, y);
}

void Edge::setXY(const std::vector<double>& x, const std::vector<double>& y)
{
	m_x = x;
	m_y = y;
}

int Edge::findNearestLine(double x, double y)
{
	int edgeId = 0;
	double minDistSquared;
	double resultX, resultY;
	for (int i = 0; i < m_x.size() - 1; ++i) {
		nearestPoint(x, y, i, &resultX, &resultY);
		double distSquared = distanceSquared(x, y, resultX, resultY);
		if (i == 0 || distSquared < minDistSquared) {
			minDistSquared = distSquared;
			edgeId = i;
		}
	}
	return edgeId;
}

QVector2D Edge::lineDirection(int lineId) const
{
	double x1 = m_x.at(lineId);
	double y1 = m_y.at(lineId);
	double x2 = m_x.at(lineId + 1);
	double y2 = m_y.at(lineId + 1);
	QVector2D ret(x2 - x1, y2 - y1);

	return ret.normalized();
}

void Edge::findMappedPoint(double x, double y, int lineIdHint, double* mappedX, double* mappedY, int* mappedLine) const
{
	*mappedLine = lineIdHint;
	nearestPoint(x, y, lineIdHint, mappedX, mappedY);
	double minDistanceSquared = distanceSquared(x, y, *mappedX, *mappedY);
	if (lineIdHint > 0) {
		double tmpMappedX, tmpMappedY;
		nearestPoint(x, y, lineIdHint - 1, &tmpMappedX, &tmpMappedY);
		double tmpMinDistanceSquared = distanceSquared(x, y, tmpMappedX, tmpMappedY);
		if (tmpMinDistanceSquared < minDistanceSquared) {
			minDistanceSquared = tmpMinDistanceSquared;
			*mappedLine = lineIdHint - 1;
			*mappedX = tmpMappedX;
			*mappedY = tmpMappedY;
		}
	}
	if (lineIdHint < m_x.size() - 2) {
		double tmpMappedX, tmpMappedY;
		nearestPoint(x, y, lineIdHint + 1, &tmpMappedX, &tmpMappedY);
		double tmpMinDistanceSquared = distanceSquared(x, y, tmpMappedX, tmpMappedY);
		if (tmpMinDistanceSquared < minDistanceSquared) {
			minDistanceSquared = tmpMinDistanceSquared;
			*mappedLine = lineIdHint + 1;
			*mappedX = tmpMappedX;
			*mappedY = tmpMappedY;
		}
	}
}

double Edge::distanceSquared(double x1, double y1, double x2, double y2)
{
	double dx = x2 - x1;
	double dy = y2 - y1;
	return dx * dx + dy * dy;
}

void Edge::nearestPoint(double x, double y, int lineId, double* resultx, double* resulty) const
{
	double ax = m_x.at(lineId);
	double ay = m_y.at(lineId);
	double bx = m_x.at(lineId + 1);
	double by = m_y.at(lineId + 1);

	nearestPoint(ax, ay, bx, by, x, y, resultx, resulty);
}

void Edge::nearestPoint(double ax, double ay, double bx, double by, double px, double py, double* resultx, double* resulty) const
{
	double abx = bx - ax;
	double aby = by - ay;
	double apx = px - ax;
	double apy = py - ay;

	double ab_len = sqrt(abx * abx + aby * aby);
	double abunitx = abx / ab_len;
	double abunity = aby / ab_len;

	double r = (apx * abunitx + apy * abunity) / ab_len;
	if (r < 0) {
		*resultx = ax;
		*resulty = ay;
		return;
	}
	if (r > 1) {
		*resultx = bx;
		*resulty = by;
		return;
	}
	*resultx = ax + r * abx;
	*resulty = ay + r * aby;
}
