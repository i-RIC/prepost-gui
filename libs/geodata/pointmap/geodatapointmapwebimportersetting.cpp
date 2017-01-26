#include "geodatapointmapwebimportersetting.h"
#include "private/geodatapointmapwebimportersetting_impl.h"

#include <misc/stringtool.h>

#include <QUrlQuery>

GeoDataPointmapWebImporterSetting::Impl::Impl(const std::string& setting) :
	m_setting (setting)
{
	QUrlQuery query(setting.c_str());
	m_caption = query.queryItemValue("caption");
	m_minZoomLevel = query.queryItemValue("minZoom").toInt();
	m_maxZoomLevel = query.queryItemValue("maxZoom").toInt();
	m_url = query.queryItemValue("url");
}

GeoDataPointmapWebImporterSetting::GeoDataPointmapWebImporterSetting() :
	GeoDataPointmapWebImporterSetting("")
{}

GeoDataPointmapWebImporterSetting::GeoDataPointmapWebImporterSetting(const std::string& setting) :
	impl {new Impl(setting)}
{}

GeoDataPointmapWebImporterSetting::GeoDataPointmapWebImporterSetting(const GeoDataPointmapWebImporterSetting& setting) :
	GeoDataPointmapWebImporterSetting()
{
	*impl = *(setting.impl);
}

GeoDataPointmapWebImporterSetting::~GeoDataPointmapWebImporterSetting()
{
	delete impl;
}

GeoDataPointmapWebImporterSetting& GeoDataPointmapWebImporterSetting::operator=(const GeoDataPointmapWebImporterSetting& setting)
{
	*impl = *(setting.impl);

	return *this;
}

std::string GeoDataPointmapWebImporterSetting::setting() const
{
	QUrlQuery query(impl->m_setting.c_str());

	return iRIC::toStr(query.toString(QUrl::FullyEncoded));
}

QString GeoDataPointmapWebImporterSetting::caption() const
{
	return impl->m_caption;
}

int GeoDataPointmapWebImporterSetting::minZoomLevel() const
{
	return impl->m_minZoomLevel;
}

int GeoDataPointmapWebImporterSetting::maxZoomLevel() const
{
	return impl->m_maxZoomLevel;
}

QString GeoDataPointmapWebImporterSetting::url() const
{
	return impl->m_url;
}
