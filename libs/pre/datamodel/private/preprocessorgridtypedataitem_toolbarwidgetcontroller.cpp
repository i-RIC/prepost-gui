#include "preprocessorgridtypedataitem_toolbarwidgetcontroller.h"
#include "preprocessorgridtypedataitem_applycolormapsettingandrendercommand.h"

#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingmodifycommand.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidgeti.h>

PreProcessorGridTypeDataItem::ToolBarWidgetController::ToolBarWidgetController(const std::string& name, ColorMapSettingToolBarWidgetI* widget, PreProcessorGridTypeDataItem* item) :
	ColorMapSettingToolBarWidgetController {widget},
	m_name {name},
	m_item {item}
{
	connect(m_widget, &ColorMapSettingToolBarWidgetI::updated, this, &ToolBarWidgetController::handleWidgetUpdate);
}

void PreProcessorGridTypeDataItem::ToolBarWidgetController::handleWidgetUpdate()
{
	auto command = new ColorMapSettingModifyCommand(m_widget->modifiedSetting(), m_item->colorMapSetting(m_name));
	m_item->pushCommand(new ApplyColorMapSettingAndRenderCommand(command, m_name, false, m_item));
}
