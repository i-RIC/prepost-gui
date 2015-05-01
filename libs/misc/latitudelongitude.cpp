#include "latitudelongitude.h"
#include <cmath>

double LatitudeLongitudeAngle::value() const
{
	double tmpdbl = m_angle + m_minute / 60. + m_second / 3600.;
	if (m_flag) {
		return tmpdbl;
	} else {
		return - tmpdbl;
	}
}

void LatitudeLongitudeAngle::setValue(double value)
{
	if (value >= 0) {
		m_flag = true;
	} else {
		m_flag = false;
		value = - value;
	}
	m_angle = static_cast<int>(std::floor(value));
	double d = (value - m_angle) * 60;
	m_minute = static_cast<int>(std::floor(d));
	m_second = (d - m_minute) * 60;
}

