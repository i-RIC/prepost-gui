#include "colormapsettingeditwidgeti.h"

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

	setupWidget();
}
