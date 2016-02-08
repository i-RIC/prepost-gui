#include "tmsrequesthandleropenstreetmap.h"

using namespace tmsloader;

TmsRequestHandlerOpenStreetMap::TmsRequestHandlerOpenStreetMap(const QPointF& centerLonLat, const QSize& size, double scale, int requestId, QNetworkAccessManager* manager, QObject *parent) :
	TmsRequestHandler {centerLonLat, size, scale, "openstreetmap.html", requestId, manager, parent}
{
	setup();
}
