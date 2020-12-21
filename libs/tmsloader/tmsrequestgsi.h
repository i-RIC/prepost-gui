#ifndef TMSLOADER_TMSREQUESTGSI_H
#define TMSLOADER_TMSREQUESTGSI_H

#include "tmsloader_api.h"
#include "tmsrequest.h"

namespace tmsloader {

class TMSLOADER_API TmsRequestGSI : public TmsRequest
{
public:
	enum class TileType {
		STD,
		PALE,
		ENGLISH,
		RELIEF,
		ORT
	};

	TmsRequestGSI(const QPointF& centerLonLat, const QSize& size, double scale, TileType tileType = TileType::STD);
	~TmsRequestGSI();

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
	#include "private/tmsrequestgsi_impl.h"
#endif // _DEBUG

#endif // TMSLOADER_TMSREQUESTGSI_H
