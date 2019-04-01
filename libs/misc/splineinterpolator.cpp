#include "mathsupport.h"
#include "splineinterpolator.h"

SplineInterpolator::SplineInterpolator(const std::vector<QPointF>& points) :
	m_points (points)
{
	buildParameters();
}

QPointF SplineInterpolator::interpolate(int i, double r)
{
	r *= m_dist[i];

	double x = m_xA[i] + m_xB[i] * r + m_xC[i] * r * r + m_xD[i] * r * r * r;
	double y = m_yA[i] + m_yB[i] * r + m_yC[i] * r * r + m_yD[i] * r * r * r;
	return QPointF(x, y);
}

void SplineInterpolator::buildParameters()
{
	size_t size = m_points.size();
	m_xA.assign(size, 0);
	m_xB.assign(size, 0);
	m_xC.assign(size, 0);
	m_xD.assign(size, 0);
	m_yA.assign(size, 0);
	m_yB.assign(size, 0);
	m_yC.assign(size, 0);
	m_yD.assign(size, 0);
	m_dist.assign(size - 1, 0);

	unsigned int i;
	for (i = 0; i < size - 1; ++i) {
		const auto& p1 = m_points[i];
		const auto& p2 = m_points[i + 1];
		m_dist[i] = iRIC::length(p2 - p1);
		m_xA[i] = m_points[i].x();
		m_yA[i] = m_points[i].y();
	}
	m_xA[i] = m_points[i].x();
	m_yA[i] = m_points[i].y();

	std::vector<double> alfax, alfay, lx, ly, mx, my, zx, zy;
	alfax.assign(size, 0);
	alfay.assign(size, 0);
	lx.assign(size, 0);
	ly.assign(size, 0);
	mx.assign(size, 0);
	my.assign(size, 0);
	zx.assign(size, 0);
	zy.assign(size, 0);

	for (i = 1; i < size - 1; ++i) {
		alfax[i] =
				3.0 / m_dist[i] * (m_xA[i + 1] - m_xA[i]) -
				3.0 / m_dist[i - 1] * (m_xA[i] - m_xA[i - 1]);
		alfay[i] =
				3.0 / m_dist[i] * (m_yA[i + 1] - m_yA[i]) -
				3.0 / m_dist[i - 1] * (m_yA[i] - m_yA[i - 1]);
	}
	lx[0] = 1;
	ly[0] = 1;
	mx[0] = 0;
	my[0] = 0;
	zx[0] = 0;
	zy[0] = 0;

	for (i = 1; i < size - 1; ++i) {
		lx[i] = 2 * (m_dist[i - 1] + m_dist[i]) - m_dist[i - 1] * mx[i - 1];
		ly[i] = 2 * (m_dist[i - 1] + m_dist[i]) - m_dist[i - 1] * my[i - 1];
		mx[i] = m_dist[i] / lx[i];
		my[i] = m_dist[i] / ly[i];
		zx[i] = (alfax[i] - m_dist[i - 1] * zx[i - 1]) / lx[i];
		zy[i] = (alfay[i] - m_dist[i - 1] * zy[i - 1]) / ly[i];
	}
	lx[size - 1] = 1;
	ly[size - 1] = 1;
	zx[size - 1] = 0;
	zy[size - 1] = 0;
	m_xC[size - 1] = 0;
	m_xC[size - 1] = 0;

	i = size - 2;
	while (true) {
		m_xC[i] = zx[i] - mx[i] * m_xC[i + 1];
		m_yC[i] = zy[i] - my[i] * m_yC[i + 1];
		m_xB[i] =
			(m_xA[i + 1] - m_xA[i]) / m_dist[i] -
			m_dist[i] * (m_xC[i + 1] + 2 * m_xC[i]) / 3.;
		m_yB[i] =
			(m_yA[i + 1] - m_yA[i]) / m_dist[i] -
			m_dist[i] * (m_yC[i + 1] + 2 * m_yC[i]) / 3.;
		m_xD[i] = (m_xC[i + 1] - m_xC[i]) / 3. / m_dist[i];
		m_yD[i] = (m_yC[i + 1] - m_yC[i]) / 3. / m_dist[i];
		if (i == 0) {break;}
		-- i;
	}
}
