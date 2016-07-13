#ifndef TMSLOADER_TMSREQUESTHANDLERBING_H
#define TMSLOADER_TMSREQUESTHANDLERBING_H

#include "tmsrequesthandler.h"
#include "../tmsrequestbing.h"

namespace tmsloader {

class TmsRequestHandlerBing : public TmsRequestHandler
{
public:
	TmsRequestHandlerBing(TmsRequestBing::ImagerySet imagerySet, const QPointF& centerLonLat, const QSize& size, double scale, int requestId, QWebEngineView* view);
};

} // tmsloader

#endif // TMSLOADER_TMSREQUESTHANDLERBING_H
