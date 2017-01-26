#ifndef GEODATAPOINTMAPWEBIMPORTERSETTING_H
#define GEODATAPOINTMAPWEBIMPORTERSETTING_H

#include "gd_pointmap_global.h"

#include <string>

class GD_POINTMAP_EXPORT GeoDataPointmapWebImporterSetting
{
public:
	GeoDataPointmapWebImporterSetting();
	GeoDataPointmapWebImporterSetting(const std::string& setting);
	GeoDataPointmapWebImporterSetting(const GeoDataPointmapWebImporterSetting& setting);

	~GeoDataPointmapWebImporterSetting();

	GeoDataPointmapWebImporterSetting& operator=(const GeoDataPointmapWebImporterSetting& setting);

	std::string setting() const;
	QString caption() const;

	int minZoomLevel() const;
	int maxZoomLevel() const;
	QString url() const;

private:
	class Impl;
	Impl* impl;
};

#endif // GEODATAPOINTMAPWEBIMPORTERSETTING_H
