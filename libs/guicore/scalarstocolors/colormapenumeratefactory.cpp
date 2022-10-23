#include "colormapenumeratefactory.h"
#include "colormapenumeratesettingcontainer.h"
#include "colormapenumeratesettingeditwidget.h"

ColorMapEnumerateFactory::ColorMapEnumerateFactory()
{}

ColorMapSettingContainerI* ColorMapEnumerateFactory::createSettingContainer()
{
	return new ColorMapEnumerateSettingContainer();
}

ColorMapSettingEditWidgetI* ColorMapEnumerateFactory::createSettingEditWidget(QWidget* parent)
{
	return new ColorMapEnumerateSettingEditWidget(parent);
}
