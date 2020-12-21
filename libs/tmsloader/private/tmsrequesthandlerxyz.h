#ifndef TMSLOADER_TMSREQUESTHANDLERXYZ_H
#define TMSLOADER_TMSREQUESTHANDLERXYZ_H

#include "tmsrequesthandler.h"
#include "../tmsrequestgooglemap.h"

namespace tmsloader {

class TmsRequestHandlerXYZ : public TmsRequestHandler
{
public:
#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
	TmsRequestHandlerXYZ(const QString& url, const QPointF& centerLonLat, const QSize& size, double scale, int requestId, const std::map<QString, QString>& options, QWebEngineView* view);
#else
	TmsRequestHandlerXYZ(const QString& url, const QPointF& centerLonLat, const QSize& size, double scale, int requestId, const std::map<QString, QString>& options, QWebView* view);
#endif
};

} // tmsloader

#endif // TMSLOADER_TMSREQUESTHANDLERXYZ_H
