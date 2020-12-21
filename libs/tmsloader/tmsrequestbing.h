#ifndef TMSLOADER_TMSREQUESTBING_H
#define TMSLOADER_TMSREQUESTBING_H

#include "tmsloader_api.h"
#include "tmsrequest.h"

namespace tmsloader {

class TMSLOADER_API TmsRequestBing : public TmsRequest
{
public:
	enum class ImagerySet {
		AERIAL,
		ROAD
	};

	TmsRequestBing(const QPointF& centerLonLat, const QSize& size, double scale, ImagerySet imagerySet = ImagerySet::AERIAL);
	~TmsRequestBing();

#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
	tmsloader::TmsRequestHandler* buildHandler(int requestId, QWebEngineView* view) const override;
#else
	tmsloader::TmsRequestHandler* buildHandler(int requestId, QWebView* view) const override;
#endif

private:
	class Impl;
	Impl* impl;
};

} // tmsloader

#ifdef _DEBUG
	#include "private/tmsrequestbing_impl.h"
#endif // _DEBUG

#endif // TMSLOADER_TMSREQUESTBING_H
