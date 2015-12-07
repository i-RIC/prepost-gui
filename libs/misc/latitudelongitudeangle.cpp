#include "latitudelongitudeangle.h"
#include "private/latitudelongitudeangle_impl.h"

#include <QString>

LatitudeLongitudeAngle::Impl::Impl(bool f, int a, int m, double s) :
	m_flag {f}, m_angle {a}, m_minute {m}, m_second {s}
{}

LatitudeLongitudeAngle::LatitudeLongitudeAngle() :
	LatitudeLongitudeAngle (false, 0, 0, 0)
{}

LatitudeLongitudeAngle::LatitudeLongitudeAngle(bool f, unsigned int a, unsigned int m, double s) :
	m_impl {new Impl(f, a, m, s)}
{}

LatitudeLongitudeAngle::LatitudeLongitudeAngle(bool f, const QString& a, const QString& m, const QString& s) :
	LatitudeLongitudeAngle()
{
	setValue(f, a, m, s);
}

LatitudeLongitudeAngle::LatitudeLongitudeAngle(double value) :
	LatitudeLongitudeAngle()
{
	setValue(value);
}

LatitudeLongitudeAngle::LatitudeLongitudeAngle(const LatitudeLongitudeAngle& angle) :
	LatitudeLongitudeAngle()
{
	*m_impl = *angle.m_impl;
}

LatitudeLongitudeAngle::LatitudeLongitudeAngle(LatitudeLongitudeAngle&& angle) :
	m_impl {angle.m_impl}
{
	angle.m_impl = nullptr;
}

LatitudeLongitudeAngle::~LatitudeLongitudeAngle()
{
	delete m_impl;
}

LatitudeLongitudeAngle& LatitudeLongitudeAngle::operator=(const LatitudeLongitudeAngle& angle)
{
	*m_impl = *angle.m_impl;

	return *this;
}

LatitudeLongitudeAngle& LatitudeLongitudeAngle::operator=(LatitudeLongitudeAngle&& angle)
{
	delete m_impl;
	m_impl = angle.m_impl;

	angle.m_impl = nullptr;

	return *this;
}

bool LatitudeLongitudeAngle::flag() const
{
	return m_impl->m_flag;
}

unsigned int LatitudeLongitudeAngle::angle() const
{
	return m_impl->m_angle;
}

unsigned int LatitudeLongitudeAngle::minute() const
{
	return m_impl->m_minute;
}

double LatitudeLongitudeAngle::second() const
{
	return m_impl->m_second;
}

void LatitudeLongitudeAngle::setFlag(bool f)
{
	m_impl->m_flag = f;
}

void LatitudeLongitudeAngle::setAngle(unsigned int a)
{
	m_impl->m_angle = a;
}

void LatitudeLongitudeAngle::setMinute(unsigned int m)
{
	m_impl->m_minute = m;
}

void LatitudeLongitudeAngle::setSecond(double s)
{
	m_impl->m_second = s;
}

double LatitudeLongitudeAngle::value() const
{
	double tmpdbl = m_impl->m_angle + m_impl->m_minute / 60. + m_impl->m_second / 3600.;
	if (m_impl->m_flag) {
		return tmpdbl;
	} else {
		return - tmpdbl;
	}
}

void LatitudeLongitudeAngle::setValue(double value)
{
	if (value >= 0) {
		m_impl->m_flag = true;
	} else {
		m_impl->m_flag = false;
		value = - value;
	}
	m_impl->m_angle = static_cast<int>(std::floor(value));
	double d = (value - m_impl->m_angle) * 60;
	m_impl->m_minute = static_cast<int>(std::floor(d));
	m_impl->m_second = (d - m_impl->m_minute) * 60;
}

void LatitudeLongitudeAngle::setValue(bool f, int a, int m, double s)
{
	m_impl->m_flag = f;
	m_impl->m_angle = a;
	m_impl->m_minute = m;
	m_impl->m_second = s;
}

void LatitudeLongitudeAngle::setValue(bool f, const QString& a, const QString& m, const QString& s)
{
	m_impl->m_flag = f;
	bool ok = true;
	m_impl->m_angle = a.toUInt(&ok, 10);
	m_impl->m_minute = m.toUInt(&ok, 10);
	m_impl->m_second = s.toDouble(&ok);
}
