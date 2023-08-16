#include "post2dwindowgridtypedataitem_toolbarwidgetcontroller.h"
#include "post2dwindowgridtypedataitem_applycolormapsettingandrendercommand.h"

#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingmodifycommand.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <guicore/scalarstocolors/delegatedcolormapsettingcontainer.h>

Post2dWindowGridTypeDataItem::ToolBarWidgetController::ToolBarWidgetController(const std::string& name, ColorMapSettingToolBarWidget* widget, Post2dWindowGridTypeDataItem* item) :
	ColorMapSettingToolBarWidgetController {widget},
	m_name {name},
	m_item {item}
{}

void Post2dWindowGridTypeDataItem::ToolBarWidgetController::handleWidgetUpdate()
{
	auto command = new ColorMapSettingModifyCommand(m_widget->modifiedSetting(), m_item->colorMapSetting(m_name)->customSetting);
	m_item->pushRenderCommand(new ApplyColorMapSettingAndRenderCommand(command, m_name, false, m_item), m_item);
}
