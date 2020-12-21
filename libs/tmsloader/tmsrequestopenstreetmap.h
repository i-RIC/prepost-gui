#ifndef TMSLOADER_TMSREQUESTOPENSTREETMAP_H
#define TMSLOADER_TMSREQUESTOPENSTREETMAP_H

#include "tmsloader_api.h"
#include "tmsrequest.h"

namespace tmsloader {

class TMSLOADER_API TmsRequestOpenStreetMap : public TmsRequest
{
public:
	TmsRequestOpenStreetMap(const QPointF& centerLonLat, const QSize& size, double scale);

#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
	TmsRequestHandler* buildHandler(int requestId, QWebEngineView* view) const override;
#else
	TmsRequestHandler* buildHandler(int requestId, QWebView* view) const override;
#endif
};

} // tmsloader

#endif // TMSLOADER_TMSREQUESTOPENSTREETMAP_H
