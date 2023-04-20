#include "colormapsettingtoolbarwidgetcontroller.h"
#include "colormapsettingtoolbarwidgeti.h"

ColorMapSettingToolBarWidgetController::ColorMapSettingToolBarWidgetController(ColorMapSettingToolBarWidgetI* widget) :
	m_widget {widget}
{
	connect(m_widget, &ColorMapSettingToolBarWidgetI::updated, this, &ColorMapSettingToolBarWidgetController::handleWidgetUpdate);
}

ColorMapSettingToolBarWidgetController::~ColorMapSettingToolBarWidgetController()
{
	delete m_widget;
}

ColorMapSettingToolBarWidgetI* ColorMapSettingToolBarWidgetController::widget() const
{
	return m_widget;
}
