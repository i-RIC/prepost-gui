#ifndef WEBMERATORUTIL_H
#define WEBMERATORUTIL_H

#include "cs_api.h"

class CoordinateSystem;

class QImage;
class QRectF;

class CS_API WebMercatorUtil
{
public:
	WebMercatorUtil(int zoomLevel);
	~WebMercatorUtil();

	void getCoordinates(int tilex, int tiley, int pixelx, int pixely, double* lon, double* lat);
	void getTileRegion(double topLeftLon, double topLeftLat, double bottomRightLon, double bottomRightLat, int* xMin, int* xMax, int* yMin, int *yMax);

	static void calcImageZoomAndSize(double lonMin, double latMin, double lonMax, double latMax, double imgWidth,
															double* lonCenter, double* latCenter, int* zoomLevel, int* width, int* height);

	static QImage convertWebMercatorToLongLat(const QRectF& rect, const QImage& image, const QString& workDir);

	/// project long-lat coordinates to x, y coordinates. 0 <= x <= 256 for zoomLevel = 0.
	static void project(double lon, double lat, double* x, double* y);
	static void unproject(double x, double y, double* lon, double* lat);
	static double pixelSize(double zoomLevel);

private:
	class Impl;
	Impl* impl;
};

#endif // WEBMERATORUTIL_H
