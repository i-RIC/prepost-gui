#include "tmsrequesthandlerxyz.h"

#include <QStringList>

using namespace tmsloader;

TmsRequestHandlerXYZ::TmsRequestHandlerXYZ(const QString& url, const QPointF& centerLonLat, const QSize& size, int zoomLevel, int requestId, const std::map<QString, QString>& options, TmsImageCache* imageCache) :
	TmsRequestHandler(centerLonLat, size, zoomLevel, "xyz.html", requestId, imageCache)
{
	std::map<QString, QString> args;
	args.insert({"%URL%", url});

	setOptions(options);
	setArgs(args);
	setup();
}
