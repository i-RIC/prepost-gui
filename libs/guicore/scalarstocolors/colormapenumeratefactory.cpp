#include "colormapenumeratefactory.h"
#include "colormapenumeratelegendsettingcontainer.h"
#include "colormapenumeratesettingcontainer.h"
#include "colormapenumeratesettingeditwidget.h"
#include "colormapenumeratesettingtoolbarwidget.h"

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

ColorMapSettingToolBarWidgetI* ColorMapEnumerateFactory::createToolbarWidget(QWidget* parent)
{
	return new ColorMapEnumerateSettingToolBarWidget(parent);
}
