#ifndef SPLINEINTERPOLATOR_H
#define SPLINEINTERPOLATOR_H

#include "misc_global.h"

#include <QPointF>

#include <vector>

class MISCDLL_EXPORT SplineInterpolator
{
public:
	SplineInterpolator(const std::vector<QPointF>& points);
	QPointF interpolate(int index, double r);

private:
	void buildParameters();

	std::vector<QPointF> m_points;

	std::vector<double> m_xA;
	std::vector<double> m_xB;
	std::vector<double> m_xC;
	std::vector<double> m_xD;
	std::vector<double> m_yA;
	std::vector<double> m_yB;
	std::vector<double> m_yC;
	std::vector<double> m_yD;
	std::vector<double> m_dist;
};

#endif // SPLINEINTERPOLATOR_H
