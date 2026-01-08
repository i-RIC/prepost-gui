#ifndef TMSLOADER_TMSUTIL_H
#define TMSLOADER_TMSUTIL_H

#include "tmsloader_api.h"

class QPointF;
class QSize;
class QSizeF;

namespace tmsloader {

class TMSLOADER_API TmsUtil
{
public:
	static double meterPerPixel(const QPointF &lonlat, double zoomLevel);
	static int calcNativeZoomLevel(const QPointF& lonLat, double meterPerPixel);
	/// Calculate the width and height of the image to cover the specified region and zoom level.
	static QSize calcPixelSize(const QPointF& lonlat, const QSizeF& size, int zoomLevel);
	static void getXYZ(const QString& url, int* z, int* x, int* y);

private:
	TmsUtil();
};

} // tmsloader

#endif // TMSLOADER_TMSUTIL_H
