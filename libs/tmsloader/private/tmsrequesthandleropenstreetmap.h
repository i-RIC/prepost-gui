#ifndef TMSLOADER_TMSREQUESTHANDLEROPENSTREETMAP_H
#define TMSLOADER_TMSREQUESTHANDLEROPENSTREETMAP_H

#include "tmsrequesthandler.h"

class TmsImageCache;

namespace tmsloader {

class TmsRequestHandlerOpenStreetMap : public TmsRequestHandler
{
public:
#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
	TmsRequestHandlerOpenStreetMap(const QPointF& centerLonLat, const QSize& size, double scale, int requestId, QWebEngineView* view, TmsImageCache* imageCache);
#else
	TmsRequestHandlerOpenStreetMap(const QPointF& centerLonLat, const QSize& size, double scale, int requestId, QWebView* view, TmsImageCache* imageCache);
#endif
};

} // tmsloader

#endif // TMSLOADER_TMSREQUESTHANDLEROPENSTREETMAP_H
