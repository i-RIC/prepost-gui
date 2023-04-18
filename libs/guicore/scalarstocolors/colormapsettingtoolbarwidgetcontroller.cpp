#include "colormapsettingtoolbarwidgetcontroller.h"
#include "colormapsettingtoolbarwidgeti.h"

ColorMapSettingToolBarWidgetController::ColorMapSettingToolBarWidgetController(ColorMapSettingToolBarWidgetI* widget) :
	m_widget {widget}
{}

ColorMapSettingToolBarWidgetController::~ColorMapSettingToolBarWidgetController()
{
	delete m_widget;
}

ColorMapSettingToolBarWidgetI* ColorMapSettingToolBarWidgetController::widget() const
{
	return m_widget;
}
