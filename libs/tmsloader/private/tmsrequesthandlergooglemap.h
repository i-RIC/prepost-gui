#ifndef TMSLOADER_TMSREQUESTHANDLERGOOGLEMAP_H
#define TMSLOADER_TMSREQUESTHANDLERGOOGLEMAP_H

#include "tmsrequesthandler.h"
#include "../tmsrequestgooglemap.h"

namespace tmsloader {

class TmsRequestHandlerGoogleMap : public TmsRequestHandler
{
public:
	TmsRequestHandlerGoogleMap(TmsRequestGoogleMap::MapType mapType, const QPointF& centerLonLat, const QSize& size, double scale, int requestId, QWebEngineView* view);
};

} // tmsloader

#endif // TMSLOADER_TMSREQUESTHANDLERGOOGLEMAP_H
