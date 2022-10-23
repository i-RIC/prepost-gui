#include "colormapfactory.h"
#include "colormapsettingcontainer.h"
#include "colormapsettingeditwidget.h"

ColorMapSettingContainerI* ColorMapFactory::createSettingContainer()
{
	return new ColorMapSettingContainer();
}

ColorMapSettingEditWidgetI* ColorMapFactory::createSettingEditWidget(QWidget* parent)
{
	return new ColorMapSettingEditWidget(parent);
}
