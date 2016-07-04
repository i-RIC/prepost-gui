#include "latitudelongitude.h"
#include "private/latitudelongitude_impl.h"

#include <QVector2D>

LatitudeLongitude::Impl::Impl(double lat, double lon) :
	m_latitude (lat), m_longitude (lon)
{}

LatitudeLongitude::LatitudeLongitude() :
	LatitudeLongitude(0, 0)
{}


LatitudeLongitude::LatitudeLongitude(double latitude, double longitude) :
	impl {new Impl(latitude, longitude)}
{}

LatitudeLongitude::LatitudeLongitude(const LatitudeLongitude& latlon) :
	LatitudeLongitude()
{
	*impl = *latlon.impl;
}

LatitudeLongitude::LatitudeLongitude(LatitudeLongitude&& latlon) :
	impl {latlon.impl}
{
	latlon.impl = nullptr;
}

LatitudeLongitude::~LatitudeLongitude()
{
	delete impl;
}

LatitudeLongitude& LatitudeLongitude::operator =(const LatitudeLongitude& latlon)
{
	*impl = *latlon.impl;

	return *this;
}

LatitudeLongitude& LatitudeLongitude::operator =(LatitudeLongitude&& latlon)
{
	delete impl;
	impl = latlon.impl;

	latlon.impl = nullptr;

	return *this;
}

LatitudeLongitudeAngle& LatitudeLongitude::latitude()
{
	return impl->m_latitude;
}

const LatitudeLongitudeAngle& LatitudeLongitude::latitude() const
{
	return impl->m_latitude;
}

LatitudeLongitudeAngle& LatitudeLongitude::longitude()
{
	return impl->m_longitude;
}

const LatitudeLongitudeAngle& LatitudeLongitude::longitude() const
{
	return impl->m_longitude;
}

void LatitudeLongitude::setLatitude(double latitude)
{
	impl->m_latitude.setValue(latitude);
}

void LatitudeLongitude::setLongitude(double longitude)
{
	impl->m_longitude.setValue(longitude);
}

QVector2D LatitudeLongitude::toQVector2D() const
{
	return QVector2D(impl->m_longitude.value(), impl->m_latitude.value());
}
