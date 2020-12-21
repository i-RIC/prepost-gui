#include "tmsrequesthandlergooglemap.h"

#include <QSettings>
#include <map>

using namespace tmsloader;

#if (QT_VERSION > QT_VERSION_CHECK(5, 5, 1))
TmsRequestHandlerGoogleMap::TmsRequestHandlerGoogleMap(TmsRequestGoogleMap::MapType mapType, const QPointF& centerLonLat, const QSize& size, double scale, int requestId, QWebEngineView* view) :
#else
TmsRequestHandlerGoogleMap::TmsRequestHandlerGoogleMap(TmsRequestGoogleMap::MapType mapType, const QPointF& centerLonLat, const QSize& size, double scale, int requestId, QWebView* view) :
#endif
	TmsRequestHandler {centerLonLat, size, scale, "googlemap.html", requestId, view}
{
	std::map<TmsRequestGoogleMap::MapType, QString> mapTypeMap;

	mapTypeMap.insert({TmsRequestGoogleMap::MapType::ROADMAP, "ROADMAP"});
	mapTypeMap.insert({TmsRequestGoogleMap::MapType::SATELLITE, "SATELLITE"});
	mapTypeMap.insert({TmsRequestGoogleMap::MapType::HYBRID, "HYBRID"});
	mapTypeMap.insert({TmsRequestGoogleMap::MapType::TERRAIN, "TERRAIN"});

	std::map<QString, QString> args;

	QSettings setting;
	QString googleMapkey = setting.value("general/googlemapskey", "").value<QString>();

	args.insert({"%MAPTYPE%", mapTypeMap[mapType]});
	args.insert({"%GOOGLEMAPKEY%", googleMapkey});

	setArgs(args);
	setup();
}
