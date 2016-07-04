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
	impl {new Impl(f, a, m, s)}
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
	*impl = *angle.impl;
}

LatitudeLongitudeAngle::LatitudeLongitudeAngle(LatitudeLongitudeAngle&& angle) :
	impl {angle.impl}
{
	angle.impl = nullptr;
}

LatitudeLongitudeAngle::~LatitudeLongitudeAngle()
{
	delete impl;
}

LatitudeLongitudeAngle& LatitudeLongitudeAngle::operator=(const LatitudeLongitudeAngle& angle)
{
	*impl = *angle.impl;

	return *this;
}

LatitudeLongitudeAngle& LatitudeLongitudeAngle::operator=(LatitudeLongitudeAngle&& angle)
{
	delete impl;
	impl = angle.impl;

	angle.impl = nullptr;

	return *this;
}

bool LatitudeLongitudeAngle::flag() const
{
	return impl->m_flag;
}

unsigned int LatitudeLongitudeAngle::angle() const
{
	return impl->m_angle;
}

unsigned int LatitudeLongitudeAngle::minute() const
{
	return impl->m_minute;
}

double LatitudeLongitudeAngle::second() const
{
	return impl->m_second;
}

void LatitudeLongitudeAngle::setFlag(bool f)
{
	impl->m_flag = f;
}

void LatitudeLongitudeAngle::setAngle(unsigned int a)
{
	impl->m_angle = a;
}

void LatitudeLongitudeAngle::setMinute(unsigned int m)
{
	impl->m_minute = m;
}

void LatitudeLongitudeAngle::setSecond(double s)
{
	impl->m_second = s;
}

double LatitudeLongitudeAngle::value() const
{
	double tmpdbl = impl->m_angle + impl->m_minute / 60. + impl->m_second / 3600.;
	if (impl->m_flag) {
		return tmpdbl;
	} else {
		return - tmpdbl;
	}
}

void LatitudeLongitudeAngle::setValue(double value)
{
	if (value >= 0) {
		impl->m_flag = true;
	} else {
		impl->m_flag = false;
		value = - value;
	}
	impl->m_angle = static_cast<int>(std::floor(value));
	double d = (value - impl->m_angle) * 60;
	impl->m_minute = static_cast<int>(std::floor(d));
	impl->m_second = (d - impl->m_minute) * 60;
}

void LatitudeLongitudeAngle::setValue(bool f, int a, int m, double s)
{
	impl->m_flag = f;
	impl->m_angle = a;
	impl->m_minute = m;
	impl->m_second = s;
}

void LatitudeLongitudeAngle::setValue(bool f, const QString& a, const QString& m, const QString& s)
{
	impl->m_flag = f;
	bool ok = true;
	impl->m_angle = a.toUInt(&ok, 10);
	impl->m_minute = m.toUInt(&ok, 10);
	impl->m_second = s.toDouble(&ok);
}
