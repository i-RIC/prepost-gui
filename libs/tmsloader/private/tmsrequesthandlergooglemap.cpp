#include "tmsrequesthandlergooglemap.h"

#include <unordered_map>

using namespace tmsloader;

TmsRequestHandlerGoogleMap::TmsRequestHandlerGoogleMap(TmsRequestGoogleMap::MapType mapType, const QPointF& centerLonLat, const QSize& size, double scale, int requestId, QNetworkAccessManager* manager, QObject *parent) :
	TmsRequestHandler {centerLonLat, size, scale, "googlemap.html", requestId, manager, parent}
{
	std::unordered_map<TmsRequestGoogleMap::MapType, QString> mapTypeMap;

	mapTypeMap.insert({TmsRequestGoogleMap::MapType::ROADMAP, "ROADMAP"});
	mapTypeMap.insert({TmsRequestGoogleMap::MapType::SATELLITE, "SATELLITE"});
	mapTypeMap.insert({TmsRequestGoogleMap::MapType::HYBRID, "HYBRID"});
	mapTypeMap.insert({TmsRequestGoogleMap::MapType::TERRAIN, "TERRAIN"});

	std::map<QString, QString> args;
	args.insert({"%MAPTYPE%", mapTypeMap[mapType]});

	setArgs(args);
	setup();
}
