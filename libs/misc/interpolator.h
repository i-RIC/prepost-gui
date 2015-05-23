#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#include <QVector2D>
#include <QVector3D>

/// Base class for interpolator
class InterpolatorBase
{
public:
	InterpolatorBase() {}
	virtual ~InterpolatorBase() {}
	/// Update parameters
	virtual void updateParameters() {}
};

/// Interpolator for one-dimensional data with one parameter
class Interpolator1D1 : public InterpolatorBase
{
public:
	Interpolator1D1() : InterpolatorBase() {}
	virtual ~Interpolator1D1() {}
	/// Get value by interpolation
	virtual double interpolate(double t) const = 0;
};

/// Interpolator for one-dimensional data with two parameters
class Interpolator1D2 : public InterpolatorBase
{
public:
	Interpolator1D2() : InterpolatorBase() {}
	virtual ~Interpolator1D2() {}
	/// Get value by interpolation
	virtual double interpolate(double s, double t) const = 0;
};

/// Interpolator for two-dimensional data with one parameter
class Interpolator2D1 : public InterpolatorBase
{
public:
	Interpolator2D1() : InterpolatorBase() {}
	virtual ~Interpolator2D1() {}
	/// Get value by interpolation
	virtual QVector2D interpolate(double t) const = 0;
	virtual Interpolator2D1* copy() const = 0;
};

/// Interpolator for two-dimensional data with two parameters
class Interpolator2D2 : public InterpolatorBase
{
public:
	Interpolator2D2() : InterpolatorBase() {}
	virtual ~Interpolator2D2() {}
	/// Get value by interpolation
	virtual QVector2D interpolate(double s, double t) const = 0;
};

/// Interpolator for three-dimensional data with one parameter
class Interpolator3D1 : public InterpolatorBase
{
public:
	Interpolator3D1() : InterpolatorBase() {}
	virtual ~Interpolator3D1() {}
	/// Get value by interpolation
	virtual QVector3D interpolate(double t) const = 0;
};

/// Interpolator for three-dimensional data with two parameters
class Interpolator3D2 : public InterpolatorBase
{
public:
	Interpolator3D2() {}
	virtual ~Interpolator3D2() {}
	/// Get value by interpolation
	virtual QVector3D interpolate(double s, double t) const = 0;
};

#endif // INTERPOLATOR_H__
