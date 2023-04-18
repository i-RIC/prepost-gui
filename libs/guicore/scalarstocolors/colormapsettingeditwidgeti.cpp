#include "colormapsettingeditwidgeti.h"
#include "colormapsettingcontaineri.h"
#include "colormaplegendsettingcontaineri.h"
#include "../image/imagesettingcontainer.h"

ColorMapSettingEditWidgetI::ColorMapSettingEditWidgetI(QWidget* parent) :
	QWidget {parent},
	m_setting {nullptr},
	m_legendSetting {nullptr}
{}

ColorMapSettingEditWidgetI::~ColorMapSettingEditWidgetI()
{}

ColorMapSettingContainerI* ColorMapSettingEditWidgetI::setting() const
{
	return m_setting;
}

void ColorMapSettingEditWidgetI::setSetting(ColorMapSettingContainerI* setting)
{
	m_setting = setting;
	m_legendSetting = nullptr;

	disconnect(m_updateImageSettingConnection);
	m_updateImageSettingConnection = connect(setting->legendSetting()->imgSetting(), &ImageSettingContainer::updated, this, &ColorMapSettingEditWidgetI::updateImageSetting);

	setupWidget();
}

ColorMapLegendSettingContainerI* ColorMapSettingEditWidgetI::legendSetting() const
{
	return m_legendSetting;
}

void ColorMapSettingEditWidgetI::setLegendSetting(ColorMapLegendSettingContainerI* setting)
{
	m_legendSetting = setting;
	m_setting = nullptr;
	disconnect(m_updateImageSettingConnection);

	if (setting->delegateMode()) {
		m_updateImageSettingConnection = connect(setting->setting()->legendSetting()->imgSetting(), &ImageSettingContainer::updated, this, &ColorMapSettingEditWidgetI::updateImageSetting);
	} else {
		m_updateImageSettingConnection = connect(setting->imgSetting(), &ImageSettingContainer::updated, this, &ColorMapSettingEditWidgetI::updateImageSetting);
	}

	setupWidget();
}
