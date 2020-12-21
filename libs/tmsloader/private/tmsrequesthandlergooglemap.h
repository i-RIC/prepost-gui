#ifndef TMSLOADER_TMSREQUESTHANDLERGOOGLEMAP_H
#define TMSLOADER_TMSREQUESTHANDLERGOOGLEMAP_H

#include "tmsrequesthandler.h"
#include "../tmsrequestgooglemap.h"

namespace tmsloader {

class TmsRequestHandlerGoogleMap : public TmsRequestHandler
{
public:
#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
	TmsRequestHandlerGoogleMap(TmsRequestGoogleMap::MapType mapType, const QPointF& centerLonLat, const QSize& size, double scale, int requestId, QWebEngineView* view);
#else
	TmsRequestHandlerGoogleMap(TmsRequestGoogleMap::MapType mapType, const QPointF& centerLonLat, const QSize& size, double scale, int requestId, QWebView* view);
#endif
};

} // tmsloader

#endif // TMSLOADER_TMSREQUESTHANDLERGOOGLEMAP_H
