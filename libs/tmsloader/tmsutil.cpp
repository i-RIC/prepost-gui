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

double TmsUtil::meterPerPixel(const QPointF& lonlat, double zoomLevel)
{
	double ret = METERPERPIXEL_AT_EQUATOR_ZOOMLEVEL1;
	ret *= std::cos(lonlat.y() / 180 * M_PI);
	ret /= std::exp(std::log(2.0) * (zoomLevel - 1));
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

void TmsUtil::getXYZ(const QString& url, int* z, int* x, int* y)
{
	*z = 0; *x = 0; *y = 0;

	QRegExp xyzRx("^https?://.+/([0-9]+)/([0-9]+)/([0-9]+)\\..+$");
	if (xyzRx.exactMatch(url)) {
		*z = xyzRx.cap(1).toInt();
		*x = xyzRx.cap(2).toInt();
		*y = xyzRx.cap(3).toInt();
	} else {
		auto frags = url.split(QRegExp("[?&]"));
		for (const auto& frag : frags) {
			auto frags2 = frag.split("=");
			if (frags2.size() < 2) {continue;}

			if (frags2.at(0) == "z") {
				*z = frags2.at(1).toInt();
			} else if (frags2.at(0) == "x") {
				*x = frags2.at(1).toInt();
			} else if (frags2.at(0) == "y") {
				*y = frags2.at(1).toInt();
			}
		}
	}
}
