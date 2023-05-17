#include "colormapfactory.h"
#include "colormaplegendsettingcontainer.h"
#include "colormapsettingcontainer.h"
#include "colormapsettingeditwidget.h"
#include "colormapsettingtoolbarwidget.h"

ColorMapSettingContainerI* ColorMapFactory::createSettingContainer()
{
	return new ColorMapSettingContainer();
}

ColorMapLegendSettingContainerI* ColorMapFactory::createLegendSettingContainer()
{
	return new ColorMapLegendSettingContainer();
}

ColorMapSettingEditWidgetI* ColorMapFactory::createSettingEditWidget(QWidget* parent)
{
	return new ColorMapSettingEditWidget(parent);
}

ColorMapSettingToolBarWidget* ColorMapFactory::createToolbarWidget(QWidget* parent)
{
	return new ColorMapSettingToolBarWidget(parent);
}
