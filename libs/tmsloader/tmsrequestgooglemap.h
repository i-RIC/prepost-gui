#ifndef TMSLOADER_TMSREQUESTGOOGLEMAP_H
#define TMSLOADER_TMSREQUESTGOOGLEMAP_H

#include "tmsloader_api.h"
#include "tmsrequest.h"

namespace tmsloader {

class TMSLOADER_API TmsRequestGoogleMap : public TmsRequest
{
public:
	enum class MapType {
		ROADMAP,
		SATELLITE,
		HYBRID,
		TERRAIN
	};
	TmsRequestGoogleMap(const QPointF& centerLonLat, const QSize& size, double scale, MapType mapType = MapType::ROADMAP);
	~TmsRequestGoogleMap();

#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
	TmsRequestHandler* buildHandler(int requestId, QWebEngineView* view) const override;
#else
	TmsRequestHandler* buildHandler(int requestId, QWebView* view) const override;
#endif

private:
	class Impl;
	Impl* impl;
};

} // tmsloader

#ifdef _DEBUG
	#include "private/tmsrequestgooglemap_impl.h"
#endif // _DEBUG

#endif // TMSLOADER_TMSREQUESTGOOGLEMAP_H
