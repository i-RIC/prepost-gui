#include "tmsimagesetting.h"
#include "private/tmsimagesetting_impl.h"

#include <misc/stringtool.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonParseError>

TmsImageSetting::TmsImageSetting() :
	impl {new Impl {}}
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

QString TmsImageSetting::setting() const
{
	QJsonObject object;
	for (const auto& pair : impl->m_settings) {
		object.insert(pair.first.c_str(), pair.second);
	}
	QJsonDocument doc(object);
	return QString(doc.toJson());
}

QString TmsImageSetting::caption() const
{
	return value("caption");
}

bool TmsImageSetting::isActive() const
{
	return value("active") == "true";
}

void TmsImageSetting::setIsActive(bool active)
{
	QString valueStr = "false";
	if (active) {
		valueStr = "true";
	}
	setValue("active", valueStr);
}

bool TmsImageSetting::isXYZ() const
{
	return value("tms") == "xyz";
}

QString TmsImageSetting::url() const
{
	return value("url");
}

int TmsImageSetting::maxZoomLevel() const
{
	auto zoomLevel = value("maxNativeZoom");
	if (zoomLevel.isNull()) {return 24;}
	return zoomLevel.toInt();
}

QString TmsImageSetting::value(const std::string& key) const
{
	auto it = impl->m_settings.find(key);
	if (it != impl->m_settings.end()) {
		return it->second;
	}

	return "";
}

void TmsImageSetting::setValue(const std::string& key, const QString& value)
{
	auto it = impl->m_settings.find(key);
	if (it != impl->m_settings.end()) {
		it->second = value;
	} else {
		impl->m_settings.insert({key, value});
	}
}

TmsImageSetting TmsImageSetting::buildFromQuery(const QString& setting)
{
	TmsImageSetting ret;

	QUrlQuery query(setting);
	auto items = query.queryItems();
	for (auto it = items.begin(); it != items.end(); ++it) {
		ret.setValue(iRIC::toStr(it->first), it->second);
	}

	return ret;
}

TmsImageSetting TmsImageSetting::buildFromString(const QString& setting)
{
	QJsonParseError error;
	auto data = QJsonDocument::fromJson(setting.toUtf8(), &error);
	if (error.error == QJsonParseError::NoError) {
		// JSON
		auto obj = data.object();
		TmsImageSetting ret;
		for (auto it = obj.begin(); it != obj.end(); ++it) {
			ret.setValue(iRIC::toStr(it.key()), it.value().toString());
		}
		return ret;
	} else {
		// URL
		return TmsImageSetting::buildFromQuery(setting);
	}
}
