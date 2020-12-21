#include "tmsrequestopenstreetmap.h"
#include "private/tmsrequesthandleropenstreetmap.h"

using namespace tmsloader;

TmsRequestOpenStreetMap::TmsRequestOpenStreetMap(const QPointF& centerLonLat, const QSize& size, double scale) :
	TmsRequest {centerLonLat, size, scale}
{}

#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
TmsRequestHandler *TmsRequestOpenStreetMap::buildHandler(int requestId, QWebEngineView* view) const
#else
TmsRequestHandler *TmsRequestOpenStreetMap::buildHandler(int requestId, QWebView* view) const
#endif
{
	return new TmsRequestHandlerOpenStreetMap(center(), size(), scale(), requestId, view);
}
