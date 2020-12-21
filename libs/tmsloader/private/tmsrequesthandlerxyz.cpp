#include "tmsrequesthandlerxyz.h"

#include <QStringList>

using namespace tmsloader;

#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
TmsRequestHandlerXYZ::TmsRequestHandlerXYZ(const QString& url, const QPointF& centerLonLat, const QSize& size, double scale, int requestId, const std::map<QString, QString>& options, QWebEngineView* view) :
#else
TmsRequestHandlerXYZ::TmsRequestHandlerXYZ(const QString& url, const QPointF& centerLonLat, const QSize& size, double scale, int requestId, const std::map<QString, QString>& options, QWebView* view) :
#endif
	TmsRequestHandler(centerLonLat, size, scale, "xyz.html", requestId, view)
{
	std::map<QString, QString> args;
	args.insert({"%URL%", url});

	setOptions(options);
	setArgs(args);
	setup();
}
