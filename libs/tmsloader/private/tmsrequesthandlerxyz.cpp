#include "tmsrequesthandlerxyz.h"

#include <QStringList>

using namespace tmsloader;

TmsRequestHandlerXYZ::TmsRequestHandlerXYZ(const QString& url, const QPointF& centerLonLat, const QSize& size, int zoomLevel, int requestId, bool lonLat, const std::map<QString, QString>& options, TmsImageCache* imageCache) :
	TmsRequestHandler(centerLonLat, size, zoomLevel, "xyz.html", requestId, lonLat, imageCache)
{
	std::map<QString, QString> args;
	args.insert({"%URL%", url});

	setOptions(options);
	setArgs(args);
	setup();
}
