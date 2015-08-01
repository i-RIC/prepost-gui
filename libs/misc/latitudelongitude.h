#ifndef __LATITUDELONGITUDE_H__
#define __LATITUDELONGITUDE_H__

#include "misc_global.h"

class QString;
class QVector2D;

/// This class contains the data of angle in Latitude-Longitude coordinate system
class MISCDLL_EXPORT LatitudeLongitudeAngle
{

public:
	/// Constructor (without initializing)
	LatitudeLongitudeAngle() {
		m_flag = false; m_angle = 0; m_minute = 0; m_second = 0;
	}
	/// Constructor with initializing
	LatitudeLongitudeAngle(bool f, unsigned int a, unsigned int m, double s) {
		setValue(f, a, m, s);
	}
	/// Constructor with initializing
	LatitudeLongitudeAngle(bool f, const QString& a, const QString& m, const QString& s) {
		setValue(f, a, m, s);
	}
	/// Constructor with initializing
	LatitudeLongitudeAngle(double value) {
		setValue(value);
	}
	/// True for North / East, false for South / West
	bool flag() const {return m_flag;}
	/// Angle
	unsigned int angle() const {return m_angle;}
	/// Minute
	unsigned int minute() const {return m_minute;}
	/// Second
	double second() const {return m_second;}
	/// Set flag value
	void setFlag(bool f) {m_flag = f;}
	/// Set angle value
	void setAngle(unsigned int a) {m_angle = a;}
	/// Set minute value
	void setMinute(unsigned int m) {m_minute = m;}
	/// Set second value
	void setSecond(double s) {m_second = s;}
	/// Return value as double presition real value
	double value() const ;
	/// Set value as a real value
	void setValue(double value);
	/// Set value with values separeted as flag, angle, minute and second
	void setValue(bool f, int a, int m, double s) {
		m_flag = f;
		m_angle = a;
		m_minute = m;
		m_second = s;
	}
	/// Set value with values separeted as flag, angle, minute and second (as strings)
	void setValue(bool f, const QString& a, const QString& m, const QString& s);

protected:
	/// true for North / East, false for South / West
	bool m_flag;
	/// Angle
	unsigned int m_angle;
	/// Minute
	unsigned int m_minute;
	/// Second
	double m_second;
};

/// This class contains a position in Latitude-Longitude coordinate system
class MISCDLL_EXPORT LatitudeLongitude
{

public:
	/// Constructor without initializing()
	LatitudeLongitude() : m_latitude(), m_longitude()
	{}

	/// Constructor with initializing
	LatitudeLongitude(double latitude, double longitude):
		m_latitude(latitude), m_longitude(longitude)
	{}
	/// Latitude
	LatitudeLongitudeAngle& latitude() {return m_latitude;}
	/// Latitude (for const)
	const LatitudeLongitudeAngle& latitude() const {return m_latitude;}
	/// Longitude
	LatitudeLongitudeAngle& longitude() {return m_longitude;}
	/// Longitude (for const)
	const LatitudeLongitudeAngle& longitude() const {return m_longitude;}
	/// Set latitude value
	void setLatitude(double latitude) {
		m_latitude.setValue(latitude);
	}
	/// Set longitude value
	void setLongitude(double longitude) {
		m_longitude.setValue(longitude);
	}
	QVector2D toQVector2D() const;

protected:
	LatitudeLongitudeAngle m_latitude;
	LatitudeLongitudeAngle m_longitude;
};

#endif // __LATITUDELONGITUDE_H__
