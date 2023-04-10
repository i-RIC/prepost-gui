#include "colormapenumeratefactory.h"
#include "colormapenumeratelegendsettingcontainer.h"
#include "colormapenumeratesettingcontainer.h"
#include "colormapenumeratesettingeditwidget.h"

ColorMapSettingContainerI* ColorMapEnumerateFactory::createSettingContainer()
{
	return new ColorMapEnumerateSettingContainer();
}

ColorMapLegendSettingContainerI* ColorMapEnumerateFactory::createLegendSettingContainer()
{
	return new ColorMapEnumerateLegendSettingContainer();
}

ColorMapSettingEditWidgetI* ColorMapEnumerateFactory::createSettingEditWidget(QWidget* parent)
{
	return new ColorMapEnumerateSettingEditWidget(parent);
}
