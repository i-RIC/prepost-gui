#include "tmsutil.h"

#include <QPointF>
#include <QSize>
#include <QSizeF>

#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>

using namespace tmsloader;

// Meter per pixel at equator for zoom level 1 = 40075334.2563 / 512.;
const double METERPERPIXEL_AT_EQUATOR_ZOOMLEVEL1 = 78272.137219;

double TmsUtil::meterPerPixel(const QPointF& lonlat, int zoomLevel)
{
	double ret = METERPERPIXEL_AT_EQUATOR_ZOOMLEVEL1;
	ret *= std::cos(lonlat.y() / 180 * M_PI);
	for (int i = 1; i < zoomLevel; ++i) {
		ret /= 2;
	}
	return ret;
}

int TmsUtil::calcNativeZoomLevel(const QPointF& lonlat, double targetMeterPerPixel)
{
	int zoomLevel = 1;
	double meterPerPixel = METERPERPIXEL_AT_EQUATOR_ZOOMLEVEL1;
	meterPerPixel *= std::cos(lonlat.y() / 180 * M_PI);

	while (meterPerPixel > targetMeterPerPixel) {
		++ zoomLevel;
		meterPerPixel /= 2.;
	}
	return zoomLevel;
}

QSize TmsUtil::calcPixelSize(const QPointF& lonlat, const QSizeF& size, int zoomLevel)
{
	double mpp = meterPerPixel(lonlat, zoomLevel);
	int width = static_cast<int>(std::ceil(size.width() / mpp));
	int height = static_cast<int>(std::ceil(size.height() / mpp));
	return QSize(width, height);
}
