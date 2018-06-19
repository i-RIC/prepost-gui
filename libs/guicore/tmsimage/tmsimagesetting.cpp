#include "tmsimagesetting.h"
#include "private/tmsimagesetting_impl.h"

#include <misc/stringtool.h>

#include <QUrlQuery>

TmsImageSetting::Impl::Impl(const std::string& setting) :
	m_setting (setting)
{
	QUrlQuery query(setting.c_str());
	m_caption = query.queryItemValue("caption");

	auto active = query.queryItemValue("active");
	m_isActive = (active == "true");
}

TmsImageSetting::TmsImageSetting() :
	TmsImageSetting("")
{}

TmsImageSetting::TmsImageSetting(const std::string& setting) :
	impl {new Impl(setting)}
{}

TmsImageSetting::TmsImageSetting(const TmsImageSetting& setting) :
	TmsImageSetting()
{
	*impl = *(setting.impl);
}

TmsImageSetting::~TmsImageSetting()
{
	delete impl;
}

TmsImageSetting& TmsImageSetting::operator=(const TmsImageSetting& setting)
{
	*impl = *(setting.impl);

	return *this;
}

std::string TmsImageSetting::setting() const
{
	QString activeStr;
	if (impl->m_isActive) {
		activeStr = "true";
	} else {
		activeStr = "false";
	}

	QUrlQuery query(impl->m_setting.c_str());
	query.removeQueryItem("active");
	query.addQueryItem("active", activeStr);

	return iRIC::toStr(query.toString(QUrl::FullyEncoded));
}

QString TmsImageSetting::caption() const
{
	return impl->m_caption;
}

bool TmsImageSetting::isActive() const
{
	return impl->m_isActive;
}

void TmsImageSetting::setIsActive(bool active)
{
	impl->m_isActive = active;
}

bool TmsImageSetting::isXYZ() const
{
	QUrlQuery query(impl->m_setting.c_str());
	return query.queryItemValue("tms") == "xyz";
}

QString TmsImageSetting::url() const
{
	QUrlQuery query(impl->m_setting.c_str());
	return query.queryItemValue("url");
}

int TmsImageSetting::maxZoomLevel() const
{
	QUrlQuery query(impl->m_setting.c_str());
	QString zoomLevel = query.queryItemValue("maxNativeZoom");
	if (zoomLevel.isNull()) {return 24;}
	return zoomLevel.toInt();
}
