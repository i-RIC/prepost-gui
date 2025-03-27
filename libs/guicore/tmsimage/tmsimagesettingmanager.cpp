#include "tmsimagesetting.h"
#include "tmsimagesettingmanager.h"

#include <tmsloader/tmsrequestxyz.h>

#include <misc/stringtool.h>

#include <QLocale>
#include <QSettings>
#include <QUrl>
#include <QUrlQuery>

using namespace tmsloader;

namespace {

TmsImageSetting buildSetting(const std::string& setting, const QString& caption, bool active = true)
{
	std::string s = setting;
	if (active) {
		s.append("&active=true");
	} else {
		s.append("&active=false");
	}
	QUrlQuery query(s.c_str());
	query.addQueryItem("caption", caption);

	return TmsImageSetting::buildFromQuery(query.toString(QUrl::FullyEncoded));
}

TmsImageSetting buildGoogleMapSetting(const std::string& url, const QString& caption)
{
	TmsImageSetting setting;

	setting.setValue("tms", "xyz");
	setting.setValue("url", url.c_str());
	setting.setValue("caption", caption);
	setting.setValue("active", "true");
	setting.setValue("maxNativeZoom", "18");

	return setting;
}

std::vector<TmsImageSetting> standardSettings()
{
	std::vector<TmsImageSetting> ret;

	// Google Map
	ret.push_back(buildGoogleMapSetting("https://mt1.google.com/vt/lyrs=m&x={x}&y={y}&z={z}", TmsImageSettingManager::tr("Google Maps")));
	ret.push_back(buildGoogleMapSetting("https://mt1.google.com/vt/lyrs=r&x={x}&y={y}&z={z}", TmsImageSettingManager::tr("Google Streets")));
	ret.push_back(buildGoogleMapSetting("https://mt1.google.com/vt/lyrs=s&x={x}&y={y}&z={z}", TmsImageSettingManager::tr("Google Satellite")));
	ret.push_back(buildGoogleMapSetting("https://mt1.google.com/vt/lyrs=y&x={x}&y={y}&z={z}", TmsImageSettingManager::tr("Google Satellite Hybrid")));
	ret.push_back(buildGoogleMapSetting("https://mt1.google.com/vt/lyrs=h&x={x}&y={y}&z={z}", TmsImageSettingManager::tr("Google Roads")));
	ret.push_back(buildGoogleMapSetting("https://mt1.google.com/vt/lyrs=t&x={x}&y={y}&z={z}", TmsImageSettingManager::tr("Google Terrain")));

	// Open Street Map
	ret.push_back(buildSetting("tms=xyz&url=https://tile.openstreetmap.jp/{z}/{x}/{y}.png&maxNativeZoom=24", TmsImageSettingManager::tr("Open Street Map"), true));

	QSettings settings;
	QString locale = settings.value("general/locale", QLocale::system().name()).value<QString>();
	bool japanese = (locale == "ja_JP");

	// GSI
	ret.push_back(buildSetting("tms=xyz&url=https://cyberjapandata.gsi.go.jp/xyz/std/{z}/{x}/{y}.png&maxNativeZoom=18", TmsImageSettingManager::tr("GSI (Standard) (Japan only)"), japanese));
	ret.push_back(buildSetting("tms=xyz&url=https://cyberjapandata.gsi.go.jp/xyz/pale/{z}/{x}/{y}.png&maxNativeZoom=18", TmsImageSettingManager::tr("GSI (Pale) (Japan only)"), japanese));
	ret.push_back(buildSetting("tms=xyz&url=https://cyberjapandata.gsi.go.jp/xyz/english/{z}/{x}/{y}.png&maxNativeZoom=11", TmsImageSettingManager::tr("GSI (English) (Japan only)"), japanese));
	ret.push_back(buildSetting("tms=xyz&url=https://cyberjapandata.gsi.go.jp/xyz/relief/{z}/{x}/{y}.png&maxNativeZoom=15", TmsImageSettingManager::tr("GSI (Relief) (Japan only)"), japanese));
	ret.push_back(buildSetting("tms=xyz&url=https://cyberjapandata.gsi.go.jp/xyz/ort/{z}/{x}/{y}.jpg&maxNativeZoom=18", TmsImageSettingManager::tr("GSI (Ortho images) (Japan only)"), japanese));

	return ret;
}

std::vector<TmsImageSetting> loadSettings()
{
	std::vector<TmsImageSetting> ret;

	QSettings qs;
	QStringList slist = qs.value("tmsimage/customsettings", QStringList()).toStringList();

	for (auto str : slist) {
		ret.push_back(TmsImageSetting::buildFromString(str));
	}

	return ret;
}

void saveSettings(const std::vector<TmsImageSetting>& settings)
{
	QStringList slist;
	for (auto s : settings) {
		slist.push_back(s.setting());
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

TmsImageSetting TmsImageSettingManager::setupXYZSetting(const QString& caption, const QString& url, int maxZoom)
{
	TmsImageSetting ret;

	ret.setValue("tms", "xyz");
	ret.setValue("url", url);
	ret.setValue("maxNativeZoom", QString::number(maxZoom));
	ret.setValue("caption", caption);

	return ret;
}

TmsRequest* TmsImageSettingManager::buildRequest(const QPointF& centerLonLat, const QSize& size, int zoomLevel, bool lonLat, const TmsImageSetting& setting) const
{
	QString tms = setting.value("tms");
	if (tms == "xyz") {
		QString url = setting.value("url");
		std::map<QString, QString> options;
		QString maxZoom = setting.value("maxNativeZoom");
		if (! maxZoom.isNull()) {
			options.insert({"maxNativeZoom", maxZoom});
		}
		return new TmsRequestXYZ(centerLonLat, size, zoomLevel, lonLat, url, options);
	}

	return nullptr;
}

