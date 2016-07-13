#include "tmsrequesthandlerbing.h"

#include <map>

using namespace tmsloader;

TmsRequestHandlerBing::TmsRequestHandlerBing(TmsRequestBing::ImagerySet imagerySet, const QPointF& centerLonLat, const QSize& size, double scale, int requestId, QWebEngineView* view) :
	TmsRequestHandler {centerLonLat, size, scale, "bing.html", requestId, view}
{
	std::map<TmsRequestBing::ImagerySet, QString> imagerySetMap;

	imagerySetMap.insert({TmsRequestBing::ImagerySet::AERIAL, "aerial"});
	imagerySetMap.insert({TmsRequestBing::ImagerySet::ROAD, "road"});

	std::map<QString, QString> args;

	args.insert({"%IMAGERYSET%", imagerySetMap[imagerySet]});
	// The key is linked against bing map development account kskinoue@gmail.com.
	args.insert({"%KEY%", "AmZwmsZTW68uMY-qCXegUhZyDfq2YKeIMr3BOHuTxXqByppyZxOUd_ybqFfYkHtA"});

	setArgs(args);
	setup();
}
