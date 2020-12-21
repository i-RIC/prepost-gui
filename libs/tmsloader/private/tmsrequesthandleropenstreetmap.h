#ifndef TMSLOADER_TMSREQUESTHANDLEROPENSTREETMAP_H
#define TMSLOADER_TMSREQUESTHANDLEROPENSTREETMAP_H

#include "tmsrequesthandler.h"

namespace tmsloader {

class TmsRequestHandlerOpenStreetMap : public TmsRequestHandler
{
public:
#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
	TmsRequestHandlerOpenStreetMap(const QPointF& centerLonLat, const QSize& size, double scale, int requestId, QWebEngineView* view);
#else
	TmsRequestHandlerOpenStreetMap(const QPointF& centerLonLat, const QSize& size, double scale, int requestId, QWebView* view);
#endif
};

} // tmsloader

#endif // TMSLOADER_TMSREQUESTHANDLEROPENSTREETMAP_H
