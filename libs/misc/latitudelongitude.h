#ifndef LATITUDELONGITUDE_H
#define LATITUDELONGITUDE_H

#include "misc_global.h"

class LatitudeLongitudeAngle;
class QVector2D;

/// This class contains a position in Latitude-Longitude coordinate system
class MISCDLL_EXPORT LatitudeLongitude
{
public:
	LatitudeLongitude();
	LatitudeLongitude(double latitude, double longitude);
	LatitudeLongitude(const LatitudeLongitude& latlon);
	LatitudeLongitude(LatitudeLongitude&& latlon);
	~LatitudeLongitude();

	LatitudeLongitude& operator =(const LatitudeLongitude& latlon);
	LatitudeLongitude& operator =(LatitudeLongitude&& latlon);

	/// Latitude
	LatitudeLongitudeAngle& latitude();
	const LatitudeLongitudeAngle& latitude() const;

	LatitudeLongitudeAngle& longitude();
	const LatitudeLongitudeAngle& longitude() const;

	void setLatitude(double latitude);
	void setLongitude(double longitude);

	QVector2D toQVector2D() const;

private:
	class Impl;
	Impl* m_impl;
};

#ifdef _DEBUG
	#include "private/latitudelongitude_impl.h"
#endif // _DEBUG

#endif // LATITUDELONGITUDE_H
