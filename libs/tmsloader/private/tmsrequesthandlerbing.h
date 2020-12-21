#ifndef TMSLOADER_TMSREQUESTHANDLERBING_H
#define TMSLOADER_TMSREQUESTHANDLERBING_H

#include "tmsrequesthandler.h"
#include "../tmsrequestbing.h"

namespace tmsloader {

class TmsRequestHandlerBing : public TmsRequestHandler
{
public:
#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
	TmsRequestHandlerBing(TmsRequestBing::ImagerySet imagerySet, const QPointF& centerLonLat, const QSize& size, double scale, int requestId, QWebEngineView* view);
#else
	TmsRequestHandlerBing(TmsRequestBing::ImagerySet imagerySet, const QPointF& centerLonLat, const QSize& size, double scale, int requestId, QWebView* view);
#endif
};

} // tmsloader

#endif // TMSLOADER_TMSREQUESTHANDLERBING_H
