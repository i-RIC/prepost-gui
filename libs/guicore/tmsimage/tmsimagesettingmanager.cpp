#include "tmsimagesetting.h"
#include "tmsimagesettingmanager.h"

#include <tmsloader/tmsrequestbing.h>
#include <tmsloader/tmsrequestgooglemap.h>
#include <tmsloader/tmsrequestgsi.h>
#include <tmsloader/tmsrequestopenstreetmap.h>
#include <tmsloader/tmsrequestxyz.h>

#include <misc/stringtool.h>

#include <QSettings>
#include <QUrlQuery>

using namespace tmsloader;

namespace {

TmsImageSetting buildSetting(const std::string& setting, const QString& caption)
{
	QUrlQuery query(setting.c_str());
	query.addQueryItem("caption", caption);
	return TmsImageSetting(iRIC::toStr(query.toString(QUrl::FullyEncoded)));
}

std::vector<TmsImageSetting> standardSettings()
{
	std::vector<TmsImageSetting> ret;

	// Google Map
	ret.push_back(buildSetting("tms=googlemap&mapType=roadmap&active=true", TmsImageSettingManager::tr("Google Map (Road)")));
	ret.push_back(buildSetting("tms=googlemap&mapType=satellite&active=true", TmsImageSettingManager::tr("Google Map (Satellite)")));
	ret.push_back(buildSetting("tms=googlemap&mapType=hybrid&active=true", TmsImageSettingManager::tr("Google Map (Hybrid)")));
	ret.push_back(buildSetting("tms=googlemap&mapType=terrain&active=true", TmsImageSettingManager::tr("Google Map (Terrain)")));

	// Open Street Map
	ret.push_back(buildSetting("tms=openstreetmap&active=true", TmsImageSettingManager::tr("Open Street Map")));

	// GSI
	ret.push_back(buildSetting("tms=xyz&url=http://cyberjapandata.gsi.go.jp/xyz/std/{z}/{x}/{y}.png&maxNativeZoom=18&active=false", TmsImageSettingManager::tr("GSI (Standard) (Japan only)")));
	ret.push_back(buildSetting("tms=xyz&url=http://cyberjapandata.gsi.go.jp/xyz/pale/{z}/{x}/{y}.png&maxNativeZoom=18&active=false", TmsImageSettingManager::tr("GSI (Pale) (Japan only)")));
	ret.push_back(buildSetting("tms=xyz&url=http://cyberjapandata.gsi.go.jp/xyz/english/{z}/{x}/{y}.png&maxNativeZoom=11&active=false", TmsImageSettingManager::tr("GSI (English) (Japan only)")));
	ret.push_back(buildSetting("tms=xyz&url=http://cyberjapandata.gsi.go.jp/xyz/relief/{z}/{x}/{y}.png&maxNativeZoom=15&active=false", TmsImageSettingManager::tr("GSI (Relief) (Japan only)")));
	ret.push_back(buildSetting("tms=xyz&url=http://cyberjapandata.gsi.go.jp/xyz/ort/{z}/{x}/{y}.png&maxNativeZoom=18&active=false", TmsImageSettingManager::tr("GSI (Ortho images) (Japan only)")));

	return ret;
}

std::vector<TmsImageSetting> loadSettings()
{
	std::vector<TmsImageSetting> ret;

	QSettings qs;
	QStringList slist = qs.value("tmsimage/customsettings", QStringList()).toStringList();

	for (auto str : slist) {
		ret.push_back(TmsImageSetting(iRIC::toStr(str)));
	}

	return ret;
}

void saveSettings(const std::vector<TmsImageSetting>& settings)
{
	QStringList slist;
	for (auto s : settings) {
		slist.push_back(s.setting().c_str());
	}

	QSettings qs;
	qs.setValue("tmsimage/customsettings", slist);
}

} // namespace

// public interface

TmsImageSettingManager::TmsImageSettingManager()
{}

TmsImageSettingManager::~TmsImageSettingManager()
{}

std::vector<TmsImageSetting> TmsImageSettingManager::settings() const
{
	auto ret = loadSettings();
	if (ret.size() != 0) {return ret;}

	return standardSettings();
}

std::vector<TmsImageSetting> TmsImageSettingManager::defaultSettings() const
{
	return standardSettings();
}

void TmsImageSettingManager::setSettings(const std::vector<TmsImageSetting>& settings)
{
	saveSettings(settings);
}

TmsImageSetting TmsImageSettingManager::setupXYZSetting(const QString& name, const QString& url, int maxZoom)
{
	return buildSetting(iRIC::toStr(QString("tms=xyz&url=%1&maxNativeZoom=%2").arg(url).arg(maxZoom)), name);
}

TmsRequest* TmsImageSettingManager::buildRequest(QPointF& centerLonLat, QSize& size, double scale, const std::string& setting) const
{
	QUrlQuery query(setting.c_str());
	QString tms = query.queryItemValue("tms");
	if (tms == "googlemap") {
		TmsRequestGoogleMap::MapType mapType = TmsRequestGoogleMap::MapType::ROADMAP;
		QString mapTypeStr = query.queryItemValue("mapType");
		if (mapTypeStr == "roadmap") {
			mapType = TmsRequestGoogleMap::MapType::ROADMAP;
		} else if (mapTypeStr == "satellite") {
			mapType = TmsRequestGoogleMap::MapType::SATELLITE;
		} else if (mapTypeStr == "hybrid") {
			mapType = TmsRequestGoogleMap::MapType::HYBRID;
		} else if (mapTypeStr == "terrain") {
			mapType = TmsRequestGoogleMap::MapType::TERRAIN;
		}
		return new TmsRequestGoogleMap(centerLonLat, size, scale, mapType);
	} else if (tms == "openstreetmap") {
		return new TmsRequestOpenStreetMap(centerLonLat, size, scale);
	} else if (tms == "bing") {
		TmsRequestBing::ImagerySet iset = TmsRequestBing::ImagerySet::AERIAL;
		QString imagerySetStr = query.queryItemValue("imageryset");
		if (imagerySetStr == "aerial") {
			iset = TmsRequestBing::ImagerySet::AERIAL;
		} else if (imagerySetStr == "road") {
			iset = TmsRequestBing::ImagerySet::ROAD;
		}
		return new TmsRequestBing(centerLonLat, size, scale, iset);
	} else if (tms == "gsi") {
		TmsRequestGSI::TileType tileType = TmsRequestGSI::TileType::STD;
		QString tileTypeStr = query.queryItemValue("tiletype");
		if (tileTypeStr == "std") {
			tileType = TmsRequestGSI::TileType::STD;
		} else if (tileTypeStr == "pale") {
			tileType = TmsRequestGSI::TileType::PALE;
		} else if (tileTypeStr == "english") {
			tileType = TmsRequestGSI::TileType::ENGLISH;
		} else if (tileTypeStr == "relief") {
			tileType = TmsRequestGSI::TileType::RELIEF;
		} else if (tileTypeStr == "ort") {
			tileType = TmsRequestGSI::TileType::ORT;
		}
		return new TmsRequestGSI(centerLonLat, size, scale, tileType);
	} else if (tms == "xyz") {
		QString url = query.queryItemValue("url");
		std::map<QString, QString> options;
		QString maxZoom = query.queryItemValue("maxNativeZoom");
		if (! maxZoom.isNull()) {
			options.insert({"maxNativeZoom", maxZoom});
		}
		return new TmsRequestXYZ(centerLonLat, size, scale, url, options);
	}
	return nullptr;
}
