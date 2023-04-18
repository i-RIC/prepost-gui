#include "colormapsettingcontaineri.h"
#include "colormapsettingtoolbarwidgeti.h"

ColorMapSettingToolBarWidgetI::ColorMapSettingToolBarWidgetI(QWidget* parent) :
	QWidget {parent},
	m_setting {nullptr}
{}

ColorMapSettingToolBarWidgetI::~ColorMapSettingToolBarWidgetI()
{}

void ColorMapSettingToolBarWidgetI::setSetting(ColorMapSettingContainerI* setting)
{
	m_setting = setting;
	connect(m_setting, &ColorMapSettingContainerI::updated, this, &ColorMapSettingToolBarWidgetI::applySetting);

	applySetting();
}
