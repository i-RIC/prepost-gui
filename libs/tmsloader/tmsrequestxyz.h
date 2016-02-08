#ifndef TMSLOADER_TMSREQUESTXYZ_H
#define TMSLOADER_TMSREQUESTXYZ_H

#include "tmsloader_api.h"
#include "tmsrequest.h"

class QString;

namespace tmsloader {

class TMSLOADER_API TmsRequestXYZ : public TmsRequest
{
public:
	TmsRequestXYZ(const QPointF& centerLonLat, const QSize& size, double scale, const QString& url);
	~TmsRequestXYZ();

	TmsRequestHandler* buildHandler(int requestId, QNetworkAccessManager* manager, QObject* parent) const override;

private:
	class Impl;
	Impl* impl;
};

} // tmsloader

#ifdef _DEBUG
	#include "private/tmsrequestxyz_impl.h"
#endif // _DEBUG

#endif // TMSLOADER_TMSREQUESTXYZ_H
