#ifndef SPRINGSOLVER_EDGE_H
#define SPRINGSOLVER_EDGE_H

#include "../springsolver.h"

#include <vector>

class QPointF;
class QVector2D;

class SpringSolver::Edge
{
public:
	Edge();

	void setLine(const std::vector<QPointF>& line);
	void setXY(const std::vector<double>& x, const std::vector<double>& y);

	int findNearestLine(double x, double y);
	QVector2D lineDirection(int lineId) const;
	void findMappedPoint(double x, double y, int lineIdHint, double* mappedX, double* mappedY, int* mappedLine) const;
	static double distanceSquared(double x1, double y1, double x2, double y2);

private:
	void nearestPoint(double x, double y, int lineId, double* resultx, double* resulty) const;
	void nearestPoint(double ax, double ay, double bx, double by, double px, double py, double* resultx, double* resulty) const;

	std::vector<double> m_x;
	std::vector<double> m_y;
};

#endif // SPRINGSOLVER_EDGE_H
