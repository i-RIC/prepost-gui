#include "post2dwindowparticlesbasescalargroupdataitem_toolbarwidgetcontroller.h"

#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/scalarstocolors/colormapsettingmodifycommand.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidgeti.h>
#include <misc/stringtool.h>

Post2dWindowParticlesBaseScalarGroupDataItem::ToolBarWidgetController::ToolBarWidgetController(ColorMapSettingToolBarWidgetI* widget, Post2dWindowParticlesBaseScalarGroupDataItem* item) :
	ColorMapSettingToolBarWidgetController(widget),
	m_item {item}
{}

void Post2dWindowParticlesBaseScalarGroupDataItem::ToolBarWidgetController::handleWidgetUpdate()
{
	auto colorTarget = iRIC::toStr(m_item->m_setting.value);
	auto cs = m_item->colorMapSetting(colorTarget);
	if (cs == nullptr) {return;}

	auto command = new ColorMapSettingModifyCommand(m_widget->modifiedSetting(), cs);
	m_item->pushUpdateActorSettingCommand(command, m_item);
}
