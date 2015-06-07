#ifndef LINEARINTERPOLATOR_H
#define LINEARINTERPOLATOR_H

#include "misc_global.h"

#include "interpolator.h"

/// Linear interpolator for one-dimensional data with one parameter
class MISCDLL_EXPORT LinearInterpolator1D1 : public Interpolator1D1
{

public:
	LinearInterpolator1D1();
	LinearInterpolator1D1(double v0, double v1);
	virtual ~LinearInterpolator1D1() {}
	/// Set values for parameter 0 and 1.
	void setValues(double v0, double v1);
	/// Value for parameter with value 0
	double value0() const {return m_value0;}
	/// Value for parameter with value 1
	double value1() const {return m_value1;}
	double interpolate(double t) const override;

private:
	double m_value0;
	double m_value1;
};

/// Linear interpolator for two-dimensional data with one parameter
class MISCDLL_EXPORT LinearInterpolator2D1 : public Interpolator2D1
{

public:
	LinearInterpolator2D1();
	LinearInterpolator2D1(const QVector2D& v0, const QVector2D& v1);
	virtual ~LinearInterpolator2D1() {}
	/// Set values for parameter 0 and 1.
	void setValues(const QVector2D& v0, const QVector2D& v1);
	const QVector2D& vector0() const {return m_value0;}
	const QVector2D& vector1() const {return m_value1;}
	QVector2D interpolate(double t) const override;
	virtual Interpolator2D1* copy() const override;

private:
	QVector2D m_value0;
	QVector2D m_value1;
};

#endif // LINEARINTERPOLATOR_H
