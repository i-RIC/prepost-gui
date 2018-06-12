#include "geodatapointmapwebimportersettingmanager.h"
#include "geodatapointmapwebimportersetting.h"

#include <misc/stringtool.h>

#include <QSettings>
#include <QUrlQuery>

namespace {

GeoDataPointmapWebImporterSetting buildSetting(const QString& caption, int minZoom, int maxZoom, const QString& url)
{
	QUrlQuery query;
	query.addQueryItem("caption", caption);
	query.addQueryItem("minZoom", QString::number(minZoom));
	query.addQueryItem("maxZoom", QString::number(maxZoom));
	query.addQueryItem("url", url);
	return GeoDataPointmapWebImporterSetting(iRIC::toStr(query.toString(QUrl::FullyEncoded)));
}

std::vector<GeoDataPointmapWebImporterSetting> standardSettings()
{
	std::vector<GeoDataPointmapWebImporterSetting> ret;

	ret.push_back(buildSetting(GeoDataPointmapWebImporterSettingManager::tr("USGS elevation tiles (SRTM)"), 0, 13, "https://earthexplorer.usgs.gov/wms/wmts/EPSG3857/srtm/{z}/{x}/{y}.csv"));
	ret.push_back(buildSetting(GeoDataPointmapWebImporterSettingManager::tr("GSI elevation tiles (DEM5A)"), 0, 15, "http://cyberjapandata.gsi.go.jp/xyz/dem5a/{z}/{x}/{y}.txt"));
	ret.push_back(buildSetting(GeoDataPointmapWebImporterSettingManager::tr("GSI elevation tiles (DEM5B)"), 0, 15, "http://cyberjapandata.gsi.go.jp/xyz/dem5b/{z}/{x}/{y}.txt"));
	ret.push_back(buildSetting(GeoDataPointmapWebImporterSettingManager::tr("GSI elevation tiles (DEM10B)"), 0, 14, "http://cyberjapandata.gsi.go.jp/xyz/dem/{z}/{x}/{y}.txt"));

	return ret;
}

std::vector<GeoDataPointmapWebImporterSetting> loadSettings()
{
	std::vector<GeoDataPointmapWebImporterSetting> ret;

	QSettings qs;
	QStringList slist = qs.value("geodatapointmapwebimporter/customsettings", QStringList()).toStringList();

	for (auto str : slist) {
		ret.push_back(GeoDataPointmapWebImporterSetting(iRIC::toStr(str)));
	}

	return ret;
}

void saveSettings(const std::vector<GeoDataPointmapWebImporterSetting>& settings)
{
	QStringList slist;
	for (auto s : settings) {
		slist.push_back(s.setting().c_str());
	}

	QSettings qs;
	qs.setValue("geodatapointmapwebimporter/customsettings", slist);
}

} // namespace

// public interface

GeoDataPointmapWebImporterSettingManager::GeoDataPointmapWebImporterSettingManager()
{}

GeoDataPointmapWebImporterSettingManager::~GeoDataPointmapWebImporterSettingManager()
{}

std::vector<GeoDataPointmapWebImporterSetting> GeoDataPointmapWebImporterSettingManager::settings() const
{
	auto ret = loadSettings();
	if (ret.size() != 0) {return ret;}

	return standardSettings();
}

std::vector<GeoDataPointmapWebImporterSetting> GeoDataPointmapWebImporterSettingManager::defaultSettings() const
{
	return standardSettings();
}

void GeoDataPointmapWebImporterSettingManager::setSettings(const std::vector<GeoDataPointmapWebImporterSetting>& settings)
{
	saveSettings(settings);
}

GeoDataPointmapWebImporterSetting GeoDataPointmapWebImporterSettingManager::setupSetting(const QString& caption, int minZoom, int maxZoom, const QString& url)
{
	return buildSetting(caption, minZoom, maxZoom, url);
}
