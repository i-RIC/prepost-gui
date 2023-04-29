#include "colormapsettingtoolbarwidgetcontroller.h"
#include "colormapsettingtoolbarwidget.h"

ColorMapSettingToolBarWidgetController::ColorMapSettingToolBarWidgetController(ColorMapSettingToolBarWidget* widget) :
	m_widget {widget}
{
	connect(m_widget, &ColorMapSettingToolBarWidget::updated, this, &ColorMapSettingToolBarWidgetController::handleWidgetUpdate);
}

ColorMapSettingToolBarWidgetController::~ColorMapSettingToolBarWidgetController()
{
	delete m_widget;
}

ColorMapSettingToolBarWidget* ColorMapSettingToolBarWidgetController::widget() const
{
	return m_widget;
}
