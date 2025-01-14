#ifndef TMSLOADER_TMSREQUESTHANDLERXYZ_H
#define TMSLOADER_TMSREQUESTHANDLERXYZ_H

#include "tmsrequesthandler.h"

namespace tmsloader {

class TmsRequestHandlerXYZ : public TmsRequestHandler
{
public:
	TmsRequestHandlerXYZ(const QString& url, const QPointF& centerLonLat, const QSize& size, int zoomLevel, int requestId, const std::map<QString, QString>& options, TmsImageCache* imageCache);
};

} // tmsloader

#endif // TMSLOADER_TMSREQUESTHANDLERXYZ_H
