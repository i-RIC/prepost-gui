#include "latitudelongitude.h"

#include <QString>
#include <QVector2D>
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

void LatitudeLongitudeAngle::setValue(bool f, const QString& a, const QString& m, const QString& s)
{
	m_flag = f;
	bool ok = true;
	m_angle = a.toUInt(&ok, 10);
	m_minute = m.toUInt(&ok, 10);
	m_second = s.toDouble(&ok);
}

QVector2D LatitudeLongitude::toQVector2D() const
{
	return QVector2D(m_latitude.value(), m_longitude.value());
}
