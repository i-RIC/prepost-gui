#include "tmsrequesthandleropenstreetmap.h"

using namespace tmsloader;

#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
TmsRequestHandlerOpenStreetMap::TmsRequestHandlerOpenStreetMap(const QPointF& centerLonLat, const QSize& size, double scale, int requestId, QWebEngineView* view) :
#else
TmsRequestHandlerOpenStreetMap::TmsRequestHandlerOpenStreetMap(const QPointF& centerLonLat, const QSize& size, double scale, int requestId, QWebView* view) :
#endif
	TmsRequestHandler {centerLonLat, size, scale, "openstreetmap.html", requestId, view}
{
	setup();
}
