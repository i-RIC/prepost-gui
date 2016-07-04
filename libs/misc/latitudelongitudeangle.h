#ifndef LATITUDELONGITUDEANGLE_H
#define LATITUDELONGITUDEANGLE_H

#include "misc_global.h"

/// This class contains the data of angle in Latitude-Longitude coordinate system
class MISCDLL_EXPORT LatitudeLongitudeAngle
{
public:
	LatitudeLongitudeAngle();
	LatitudeLongitudeAngle(bool f, unsigned int a, unsigned int m, double s);
	LatitudeLongitudeAngle(bool f, const QString& a, const QString& m, const QString& s);
	LatitudeLongitudeAngle(double value);
	LatitudeLongitudeAngle(const LatitudeLongitudeAngle& angle);
	LatitudeLongitudeAngle(LatitudeLongitudeAngle&& angle);
	~LatitudeLongitudeAngle();

	LatitudeLongitudeAngle& operator=(const LatitudeLongitudeAngle& angle);
	LatitudeLongitudeAngle& operator=(LatitudeLongitudeAngle&& angle);

	/// True for North / East, false for South / West
	bool flag() const;

	unsigned int angle() const;
	unsigned int minute() const;
	double second() const;

	void setFlag(bool f);
	void setAngle(unsigned int a);
	void setMinute(unsigned int m);
	void setSecond(double s);

	// interfaces agains double precision value
	double value() const ;
	void setValue(double value);

	// functions to set value
	void setValue(bool f, int a, int m, double s);
	void setValue(bool f, const QString& a, const QString& m, const QString& s);

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/latitudelongitudeangle_impl.h"
#endif // _DEBUG

#endif // LATITUDELONGITUDEANGLE_H
