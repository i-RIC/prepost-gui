#ifndef LATITUDELONGITUDE_IMPL_H
#define LATITUDELONGITUDE_IMPL_H

#include "../latitudelongitude.h"
#include "../latitudelongitudeangle.h"

class LatitudeLongitude::Impl
{
public:
	Impl(double lat, double lon);

	LatitudeLongitudeAngle m_latitude;
	LatitudeLongitudeAngle m_longitude;
};

#endif // LATITUDELONGITUDE_IMPL_H
