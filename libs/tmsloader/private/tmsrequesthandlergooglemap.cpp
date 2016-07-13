#include "tmsrequesthandlergooglemap.h"

#include <map>

using namespace tmsloader;

TmsRequestHandlerGoogleMap::TmsRequestHandlerGoogleMap(TmsRequestGoogleMap::MapType mapType, const QPointF& centerLonLat, const QSize& size, double scale, int requestId, QWidget* parent) :
	TmsRequestHandler {centerLonLat, size, scale, "googlemap.html", requestId, parent}
{
	std::map<TmsRequestGoogleMap::MapType, QString> mapTypeMap;

	mapTypeMap.insert({TmsRequestGoogleMap::MapType::ROADMAP, "ROADMAP"});
	mapTypeMap.insert({TmsRequestGoogleMap::MapType::SATELLITE, "SATELLITE"});
	mapTypeMap.insert({TmsRequestGoogleMap::MapType::HYBRID, "HYBRID"});
	mapTypeMap.insert({TmsRequestGoogleMap::MapType::TERRAIN, "TERRAIN"});

	std::map<QString, QString> args;
	args.insert({"%MAPTYPE%", mapTypeMap[mapType]});

	setArgs(args);
	setup();
}
