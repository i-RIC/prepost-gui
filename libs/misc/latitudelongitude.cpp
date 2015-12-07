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
	m_impl {new Impl(latitude, longitude)}
{}

LatitudeLongitude::LatitudeLongitude(const LatitudeLongitude& latlon) :
	LatitudeLongitude()
{
	*m_impl = *latlon.m_impl;
}

LatitudeLongitude::LatitudeLongitude(LatitudeLongitude&& latlon) :
	m_impl {latlon.m_impl}
{
	latlon.m_impl = nullptr;
}

LatitudeLongitude::~LatitudeLongitude()
{
	delete m_impl;
}

LatitudeLongitude& LatitudeLongitude::operator =(const LatitudeLongitude& latlon)
{
	*m_impl = *latlon.m_impl;

	return *this;
}

LatitudeLongitude& LatitudeLongitude::operator =(LatitudeLongitude&& latlon)
{
	delete m_impl;
	m_impl = latlon.m_impl;

	latlon.m_impl = nullptr;

	return *this;
}

LatitudeLongitudeAngle& LatitudeLongitude::latitude()
{
	return m_impl->m_latitude;
}

const LatitudeLongitudeAngle& LatitudeLongitude::latitude() const
{
	return m_impl->m_latitude;
}

LatitudeLongitudeAngle& LatitudeLongitude::longitude()
{
	return m_impl->m_longitude;
}

const LatitudeLongitudeAngle& LatitudeLongitude::longitude() const
{
	return m_impl->m_longitude;
}

void LatitudeLongitude::setLatitude(double latitude)
{
	m_impl->m_latitude.setValue(latitude);
}

void LatitudeLongitude::setLongitude(double longitude)
{
	m_impl->m_longitude.setValue(longitude);
}

QVector2D LatitudeLongitude::toQVector2D() const
{
	return QVector2D(m_impl->m_longitude.value(), m_impl->m_latitude.value());
}
