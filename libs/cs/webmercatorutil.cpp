#include "webmercatorutil.h"
#include "private/webmercatorutil_impl.h"

#include <QString>

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

namespace {

double DEG_TO_RAD = M_PI / 180;
double RAD_TO_DEG = 180 / M_PI;

} // namespace

void WebMercatorUtil::Impl::init(int zoomlevel)
{
	double c = 256;
	for (int i = 0; i < zoomlevel; ++i) {
		c *= 2;
	}
	Bc = c / 360.0;
	Cc = c / (2 * M_PI);
	double e = c / 2;
	zcx = e;
	zcy = e;
}

void WebMercatorUtil::Impl::project_plxel(double lon, double lat, double* x, double* y)
{
	*x = zcx + lon * Bc;
	double f = std::sin(DEG_TO_RAD * lat);
	*y = zcy + 0.5 * std::log((1 + f) / (1 - f)) * (-Cc);
}

void WebMercatorUtil::Impl::unproject_pixel(double x, double y, double* lon, double* lat)
{
	*lon = (x - zcx) / Bc;
	double g = (y - zcy) / (- Cc);
	*lat = RAD_TO_DEG * (2 * std::atan(std::exp(g)) - 0.5 * M_PI);
}

WebMercatorUtil::WebMercatorUtil(int zoomLevel) :
	impl {new Impl ()}
{
	impl->init(zoomLevel);
}

WebMercatorUtil::~WebMercatorUtil()
{
	delete impl;
}

void WebMercatorUtil::getCoordinates(int tilex, int tiley, int pixelx, int pixely, double* lon, double* lat)
{
	double X = tilex * 256 + pixelx + 0.5;
	double Y = tiley * 256 + pixely + 0.5;

	impl->unproject_pixel(X, Y, lon, lat);
}

void WebMercatorUtil::getTileRegion(double topLeftLon, double topLeftLat, double bottomRightLon, double bottomRightLat, int* xMin, int* xMax, int* yMin, int *yMax)
{
	double xmin, ymin, xmax, ymax;

	impl->project_plxel(topLeftLon, topLeftLat, &xmin, &ymin);
	impl->project_plxel(bottomRightLon, bottomRightLat, &xmax, &ymax);

	xmin /= 256;
	ymin /= 256;
	xmax /= 256;
	ymax /= 256;

	*xMin = static_cast<int>(std::floor(xmin));
	*yMin = static_cast<int>(std::floor(ymin));
	*xMax = static_cast<int>(std::ceil(xmax));
	*yMax = static_cast<int>(std::ceil(ymax));
}
