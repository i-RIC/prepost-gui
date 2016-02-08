#include "tmsrequesthandlerbing.h"

#include <unordered_map>

using namespace tmsloader;

TmsRequestHandlerBing::TmsRequestHandlerBing(TmsRequestBing::ImagerySet imagerySet, const QPointF& centerLonLat, const QSize& size, double scale, int requestId, QNetworkAccessManager* manager, QObject *parent) :
	TmsRequestHandler {centerLonLat, size, scale, "bing.html", requestId, manager, parent}
{
	std::unordered_map<TmsRequestBing::ImagerySet, QString> imagerySetMap;

	imagerySetMap.insert({TmsRequestBing::ImagerySet::AERIAL, "Aerial"});
	imagerySetMap.insert({TmsRequestBing::ImagerySet::AERIALWITHLABELS, "AerialWithLabels"});
	imagerySetMap.insert({TmsRequestBing::ImagerySet::ROAD, "Road"});

	std::map<QString, QString> args;

	args.insert({"%IMAGERYSET%", imagerySetMap[imagerySet]});
	// The key is linked against bing map development account kskinoue@gmail.com.
	args.insert({"%KEY%", "AmZwmsZTW68uMY-qCXegUhZyDfq2YKeIMr3BOHuTxXqByppyZxOUd_ybqFfYkHtA"});

	setArgs(args);
	setup();
}
