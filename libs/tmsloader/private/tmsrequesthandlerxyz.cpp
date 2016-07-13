#include "tmsrequesthandlerxyz.h"

using namespace tmsloader;

TmsRequestHandlerXYZ::TmsRequestHandlerXYZ(const QString& url, const QPointF& centerLonLat, const QSize& size, double scale, int requestId, const std::map<QString, QString>& options, QObject *parent) :
	TmsRequestHandler(centerLonLat, size, scale, "xyz.html", requestId, parent)
{
	std::map<QString, QString> args;
	QStringList optionsStrs;
	for (auto it = options.begin(); it != options.end(); ++it) {
		optionsStrs.push_back(QString("%1:'%2'").arg(it->first).arg(it->second));
	}
	QString optionsStr = optionsStrs.join(',');

	args.insert({"%URL%", url});
	args.insert({"%OPTIONS%", optionsStr});

	setArgs(args);
	setup();
}
