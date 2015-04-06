#ifndef LINEARINTERPOLATOR_H
#define LINEARINTERPOLATOR_H

#include "misc_global.h"

#include "interpolator.h"

class MISCDLL_EXPORT LinearInterpolator1D1 : public Interpolator1D1
{
public:
	LinearInterpolator1D1();
	LinearInterpolator1D1(double v0, double v1);
	virtual ~LinearInterpolator1D1(){}
	void setValues(double v0, double v1);
	double value0() const {return m_value0;}
	double value1() const {return m_value1;}
	double interpolate(double t);

private:
	double m_value0;
	double m_value1;
};

class MISCDLL_EXPORT LinearInterpolator2D1 : public Interpolator2D1
{
public:
	LinearInterpolator2D1();
	LinearInterpolator2D1(const QVector2D& v0, const QVector2D& v1);
	virtual ~LinearInterpolator2D1(){}
	void setValues(const QVector2D& v0, const QVector2D& v1);
	QVector2D interpolate(double t);
	virtual Interpolator2D1* copy();

private:
	QVector2D m_value0;
	QVector2D m_value1;
};

#endif // LINEARINTERPOLATOR_H
