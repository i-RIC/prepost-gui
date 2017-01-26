#ifndef GEODATAPOINTMAPWEBIMPORTERSETTING_IMPL_H
#define GEODATAPOINTMAPWEBIMPORTERSETTING_IMPL_H

#include "../geodatapointmapwebimportersetting.h"

#include <QString>

class GeoDataPointmapWebImporterSetting::Impl
{
public:
	Impl(const std::string& setting);

	std::string m_setting;
	QString m_caption;

	int m_minZoomLevel;
	int m_maxZoomLevel;
	QString m_url;
};

#endif // GEODATAPOINTMAPWEBIMPORTERSETTING_IMPL_H
