#ifndef LINEARINTERPOLATOR_H
#define LINEARINTERPOLATOR_H

#include "misc_global.h"

#include "interpolator.h"

#include <QPointF>

/// Linear interpolator for one-dimensional data with one parameter
class MISCDLL_EXPORT LinearInterpolator1D1 : public Interpolator1D1
{

public:
	LinearInterpolator1D1();
	LinearInterpolator1D1(double v0, double v1);
	virtual ~LinearInterpolator1D1() override;
	double value0() const;
	double value1() const;
	/// Set values for parameter 0 and 1.
	void setValues(double v0, double v1);
	/// Value for parameter with value 0
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
	LinearInterpolator2D1(const QPointF& v0, const QPointF& v1);
	virtual ~LinearInterpolator2D1();
	const QPointF& vector0() const;
	const QPointF& vector1() const;
	/// Set values for parameter 0 and 1.
	void setValues(const QPointF& v0, const QPointF& v1);
	QPointF interpolate(double t) const override;
	virtual Interpolator2D1* copy() const override;

private:
	QPointF m_value0;
	QPointF m_value1;
};

#endif // LINEARINTERPOLATOR_H
