#include "post2dwindowgridtypedataitem_toolbarwidgetcontroller.h"
#include "post2dwindowgridtypedataitem_applycolormapsettingcommand.h"

#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingmodifycommand.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidgeti.h>
#include <guicore/scalarstocolors/delegatedcolormapsettingcontainer.h>

Post2dWindowGridTypeDataItem::ToolBarWidgetController::ToolBarWidgetController(const std::string& name, ColorMapSettingToolBarWidgetI* widget, Post2dWindowGridTypeDataItem* item) :
	ColorMapSettingToolBarWidgetController {widget},
	m_name {name},
	m_item {item}
{}

void Post2dWindowGridTypeDataItem::ToolBarWidgetController::handleWidgetUpdate()
{
	auto command = new ColorMapSettingModifyCommand(m_widget->modifiedSetting(), m_item->colorMapSetting(m_name)->customSetting);
	m_item->pushRenderCommand(new ApplyColorMapSettingCommand(command, m_name, false, m_item), m_item);
}
