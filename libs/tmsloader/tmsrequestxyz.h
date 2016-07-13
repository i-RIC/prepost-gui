#ifndef TMSLOADER_TMSREQUESTXYZ_H
#define TMSLOADER_TMSREQUESTXYZ_H

#include "tmsloader_api.h"
#include "tmsrequest.h"

#include <map>

class QString;

namespace tmsloader {

class TMSLOADER_API TmsRequestXYZ : public TmsRequest
{
public:
	TmsRequestXYZ(const QPointF& centerLonLat, const QSize& size, double scale, const QString& url, std::map<QString, QString>& options);
	~TmsRequestXYZ();

	TmsRequestHandler* buildHandler(int requestId, QWebEngineView* view) const override;

private:
	class Impl;
	Impl* impl;
};

} // tmsloader

#ifdef _DEBUG
	#include "private/tmsrequestxyz_impl.h"
#endif // _DEBUG

#endif // TMSLOADER_TMSREQUESTXYZ_H
