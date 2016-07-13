#ifndef TMSLOADER_TMSREQUESTOPENSTREETMAP_H
#define TMSLOADER_TMSREQUESTOPENSTREETMAP_H

#include "tmsloader_api.h"
#include "tmsrequest.h"

namespace tmsloader {

class TMSLOADER_API TmsRequestOpenStreetMap : public TmsRequest
{
public:
	TmsRequestOpenStreetMap(const QPointF& centerLonLat, const QSize& size, double scale);

	TmsRequestHandler* buildHandler(int requestId, QWebEngineView* view) const override;
};

} // tmsloader

#endif // TMSLOADER_TMSREQUESTOPENSTREETMAP_H
