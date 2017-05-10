#ifndef GEODATAPOINTMAPWEBIMPORTERSETTINGMANAGER_H
#define GEODATAPOINTMAPWEBIMPORTERSETTINGMANAGER_H

#include "gd_pointmap_global.h"

#include <QObject>

#include <vector>

class GeoDataPointmapWebImporterSetting;

class GD_POINTMAP_EXPORT GeoDataPointmapWebImporterSettingManager : public QObject
{
	Q_OBJECT

public:
	GeoDataPointmapWebImporterSettingManager();
	~GeoDataPointmapWebImporterSettingManager();

	std::vector<GeoDataPointmapWebImporterSetting> settings() const;
	std::vector<GeoDataPointmapWebImporterSetting> defaultSettings() const;
	void setSettings(const std::vector<GeoDataPointmapWebImporterSetting>& settings);

	GeoDataPointmapWebImporterSetting setupSetting(const QString& caption, int minZoom, int maxZoom, const QString& url);
};

#endif // GEODATAPOINTMAPWEBIMPORTERSETTINGMANAGER_H
