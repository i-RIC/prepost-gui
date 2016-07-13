#include "tmsrequesthandleropenstreetmap.h"

using namespace tmsloader;

TmsRequestHandlerOpenStreetMap::TmsRequestHandlerOpenStreetMap(const QPointF& centerLonLat, const QSize& size, double scale, int requestId, QWebView* view) :
	TmsRequestHandler {centerLonLat, size, scale, "openstreetmap.html", requestId, view}
{
	setup();
}
