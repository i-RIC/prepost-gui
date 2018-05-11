#include "tmsrequesthandlerxyz.h"

#include <QStringList>

using namespace tmsloader;

TmsRequestHandlerXYZ::TmsRequestHandlerXYZ(const QString& url, const QPointF& centerLonLat, const QSize& size, double scale, int requestId, const std::map<QString, QString>& options, QWebView* view) :
	TmsRequestHandler(centerLonLat, size, scale, "xyz.html", requestId, view)
{
	std::map<QString, QString> args;
	args.insert({"%URL%", url});

	setOptions(options);
	setArgs(args);
	setup();
}
