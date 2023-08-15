#include "measureddatapointgroupdataitem_impl.h"
#include "measureddatapointgroupdataitem_toolbarwidgetcontroller.h"

#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/scalarstocolors/colormapsettingmodifycommand.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <misc/stringtool.h>

MeasuredDataPointGroupDataItem::ToolBarWidgetController::ToolBarWidgetController(ColorMapSettingToolBarWidget* widget, MeasuredDataPointGroupDataItem* item) :
	ColorMapSettingToolBarWidgetController(widget),
	m_item {item}
{}

void MeasuredDataPointGroupDataItem::ToolBarWidgetController::handleWidgetUpdate()
{
	auto colorTarget = iRIC::toStr(m_item->impl->m_setting.value);
	auto cs = m_item->colorMapSetting(colorTarget);
	if (cs == nullptr) {return;}

	auto command = new ColorMapSettingModifyCommand(m_widget->modifiedSetting(), cs);
	m_item->pushUpdateActorSettingCommand(command, m_item);
}
