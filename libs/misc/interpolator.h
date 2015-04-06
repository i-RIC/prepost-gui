#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H

#include <QVector2D>
#include <QVector3D>

class InterpolatorBase
{
public:
	InterpolatorBase(){}
	virtual ~InterpolatorBase(){}
	virtual void updateParameters(){}
};

class Interpolator1D1 : public InterpolatorBase
{
public:
	Interpolator1D1() : InterpolatorBase(){}
	virtual ~Interpolator1D1(){}
	virtual double interpolate(double t) = 0;
};

class Interpolator1D2 : public InterpolatorBase
{
public:
	Interpolator1D2() : InterpolatorBase(){}
	virtual ~Interpolator1D2(){}
	virtual double interpolate(double s, double t) = 0;
};

class Interpolator2D1 : public InterpolatorBase
{
public:
	Interpolator2D1() : InterpolatorBase(){}
	virtual ~Interpolator2D1(){}
	virtual QVector2D interpolate(double t) = 0;
	virtual Interpolator2D1* copy() = 0;
};

class Interpolator2D2 : public InterpolatorBase
{
public:
	Interpolator2D2() : InterpolatorBase(){}
	virtual ~Interpolator2D2(){}
	virtual QVector2D interpolate(double s, double t) = 0;
};

class Interpolator3D1 : public InterpolatorBase
{
public:
	Interpolator3D1() : InterpolatorBase(){}
	virtual ~Interpolator3D1(){}
	virtual QVector3D interpolate(double t) = 0;
};

class Interpolator3D2 : public InterpolatorBase
{
public:
	Interpolator3D2(){}
	virtual ~Interpolator3D2(){}
	virtual QVector3D interpolate(double s, double t) = 0;
};

#endif // INTERPOLATOR_H__
