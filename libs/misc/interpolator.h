#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#include "misc_global.h"

class QPointF;

/// Base class for interpolator
class MISCDLL_EXPORT InterpolatorBase
{
public:
	InterpolatorBase();
	virtual ~InterpolatorBase();

	/// Update parameters
	virtual void updateParameters();
};

/// Interpolator for one-dimensional data with one parameter
class MISCDLL_EXPORT Interpolator1D1 : public InterpolatorBase
{
public:
	Interpolator1D1();
	virtual ~Interpolator1D1() override;

	/// Get value by interpolation
	virtual double interpolate(double t) const = 0;
};

/// Interpolator for one-dimensional data with two parameters
class MISCDLL_EXPORT Interpolator1D2 : public InterpolatorBase
{
public:
	Interpolator1D2();
	virtual ~Interpolator1D2();

	/// Get value by interpolation
	virtual double interpolate(double s, double t) const = 0;
};

/// Interpolator for two-dimensional data with one parameter
class MISCDLL_EXPORT Interpolator2D1 : public InterpolatorBase
{
public:
	Interpolator2D1();
	virtual ~Interpolator2D1() override;

	/// Get value by interpolation
	virtual QPointF interpolate(double t) const = 0;
	virtual Interpolator2D1* copy() const = 0;
};

/// Interpolator for two-dimensional data with two parameters
class MISCDLL_EXPORT Interpolator2D2 : public InterpolatorBase
{
public:
	Interpolator2D2();
	virtual ~Interpolator2D2() override;

	/// Get value by interpolation
	virtual QPointF interpolate(double s, double t) const = 0;
};

#endif // INTERPOLATOR_H__
