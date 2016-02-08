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
		AERIALWITHLABELS,
		ROAD
	};

	TmsRequestBing(const QPointF& centerLonLat, const QSize& size, double scale, ImagerySet imagerySet = ImagerySet::AERIAL);
	~TmsRequestBing();

	tmsloader::TmsRequestHandler *buildHandler(int requestId, QNetworkAccessManager* manager, QObject* parent) const override;

private:
	class Impl;
	Impl* impl;
};

} // tmsloader

#ifdef _DEBUG
	#include "private/tmsrequestbing_impl.h"
#endif // _DEBUG

#endif // TMSLOADER_TMSREQUESTBING_H
