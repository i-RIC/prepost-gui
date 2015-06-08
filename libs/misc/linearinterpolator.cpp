#include "linearinterpolator.h"

LinearInterpolator1D1::LinearInterpolator1D1()
	: Interpolator1D1()
{
	m_value0 = 0;
	m_value1 = 0;
}

LinearInterpolator1D1::LinearInterpolator1D1(double v0, double v1)
	: Interpolator1D1()
{
	m_value0 = v0;
	m_value1 = v1;
}

void LinearInterpolator1D1::setValues(double v0, double v1)
{
	m_value0 = v0;
	m_value1 = v1;
}

double LinearInterpolator1D1::interpolate(double t) const
{
	return t * m_value1 + (1 - t) * m_value0;
}

LinearInterpolator2D1::LinearInterpolator2D1()
	: Interpolator2D1()
{

}

LinearInterpolator2D1::LinearInterpolator2D1(const QVector2D& v0, const QVector2D& v1)
{
	m_value0 = v0;
	m_value1 = v1;
}

void LinearInterpolator2D1::setValues(const QVector2D& v0, const QVector2D& v1)
{
	m_value0 = v0;
	m_value1 = v1;
}

QVector2D LinearInterpolator2D1::interpolate(double t) const
{
	return t * m_value1 + (1 - t) * m_value0;
}

Interpolator2D1* LinearInterpolator2D1::copy() const
{
	return new LinearInterpolator2D1(m_value0, m_value1);
}
