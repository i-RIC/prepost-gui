#include "linearinterpolator.h"

#include <QPointF>

LinearInterpolator1D1::LinearInterpolator1D1() :
	Interpolator1D1(),
	m_value0 {0},
	m_value1 {0}
{}

LinearInterpolator1D1::LinearInterpolator1D1(double v0, double v1) :
	Interpolator1D1(),
	m_value0 {v0},
	m_value1 {v1}
{}

LinearInterpolator1D1::~LinearInterpolator1D1()
{}

double LinearInterpolator1D1::value0() const
{
	return m_value0;
}

double LinearInterpolator1D1::value1() const
{
	return m_value1;
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

LinearInterpolator2D1::LinearInterpolator2D1() :
	Interpolator2D1()
{}

LinearInterpolator2D1::LinearInterpolator2D1(const QPointF& v0, const QPointF& v1) :
	m_value0 {v0},
	m_value1 {v1}
{}

LinearInterpolator2D1::~LinearInterpolator2D1()
{}

const QPointF& LinearInterpolator2D1::vector0() const
{
	return m_value0;
}

const QPointF& LinearInterpolator2D1::vector1() const
{
	return m_value1;
}

void LinearInterpolator2D1::setValues(const QPointF& v0, const QPointF& v1)
{
	m_value0 = v0;
	m_value1 = v1;
}

QPointF LinearInterpolator2D1::interpolate(double t) const
{
	return t * m_value1 + (1 - t) * m_value0;
}

Interpolator2D1* LinearInterpolator2D1::copy() const
{
	return new LinearInterpolator2D1(m_value0, m_value1);
}
