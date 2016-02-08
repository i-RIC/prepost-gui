#include "tmsrequesthandlerxyz.h"

using namespace tmsloader;

TmsRequestHandlerXYZ::TmsRequestHandlerXYZ(const QString& url, const QPointF& centerLonLat, const QSize& size, double scale, int requestId, QNetworkAccessManager* manager, QObject *parent) :
	TmsRequestHandler(centerLonLat, size, scale, "xyz.html", requestId, manager, parent)
{
	std::map<QString, QString> args;
	args.insert({"%URL%", url});

	setArgs(args);
	setup();
}
