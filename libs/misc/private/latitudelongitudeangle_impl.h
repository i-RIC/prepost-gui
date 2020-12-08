#ifndef LATITUDELONGITUDEANGLE_IMPL_H
#define LATITUDELONGITUDEANGLE_IMPL_H

#include "../latitudelongitudeangle.h"

class LatitudeLongitudeAngle::Impl
{
public:
	Impl(bool f, unsigned int a, unsigned int m, double s);

	/// true for North / East, false for South / West
	bool m_flag;

	unsigned int m_angle;
	unsigned int m_minute;
	double m_second;
};

#endif // LATITUDELONGITUDEANGLE_IMPL_H
