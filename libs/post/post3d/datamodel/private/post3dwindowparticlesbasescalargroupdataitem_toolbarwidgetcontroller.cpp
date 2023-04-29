#include "post3dwindowparticlesbasescalargroupdataitem_toolbarwidgetcontroller.h"

#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/scalarstocolors/colormapsettingmodifycommand.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <misc/stringtool.h>

Post3dWindowParticlesBaseScalarGroupDataItem::ToolBarWidgetController::ToolBarWidgetController(ColorMapSettingToolBarWidget* widget, Post3dWindowParticlesBaseScalarGroupDataItem* item) :
	ColorMapSettingToolBarWidgetController(widget),
	m_item {item}
{}

void Post3dWindowParticlesBaseScalarGroupDataItem::ToolBarWidgetController::handleWidgetUpdate()
{
	auto colorTarget = iRIC::toStr(m_item->m_setting.value);
	auto cs = m_item->colorMapSetting(colorTarget);
	if (cs == nullptr) {return;}

	auto command = new ColorMapSettingModifyCommand(m_widget->modifiedSetting(), cs);
	m_item->pushUpdateActorSettingCommand(command, m_item);
}
