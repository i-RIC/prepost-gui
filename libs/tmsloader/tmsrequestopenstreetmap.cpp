#include "tmsrequestopenstreetmap.h"
#include "private/tmsrequesthandleropenstreetmap.h"

using namespace tmsloader;

TmsRequestOpenStreetMap::TmsRequestOpenStreetMap(const QPointF& centerLonLat, const QSize& size, double scale) :
	TmsRequest {centerLonLat, size, scale}
{}

TmsRequestHandler *TmsRequestOpenStreetMap::buildHandler(int requestId, QWidget* parent) const
{
	return new TmsRequestHandlerOpenStreetMap(center(), size(), scale(), requestId, parent);
}
