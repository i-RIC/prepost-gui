#ifndef WEBMERATORUTIL_H
#define WEBMERATORUTIL_H

#include "cs_api.h"

class CoordinateSystem;

class CS_API WebMercatorUtil
{
public:
	WebMercatorUtil(int zoomLevel);
	~WebMercatorUtil();

	void getCoordinates(int tilex, int tiley, int pixelx, int pixely, double* lon, double* lat);
	void getTileRegion(double topLeftLon, double topLeftLat, double bottomRightLon, double bottomRightLat, int* xMin, int* xMax, int* yMin, int *yMax);

private:
	CoordinateSystem* m_pixelCS;
	CoordinateSystem* m_tileCS;
};

#endif // WEBMERATORUTIL_H
