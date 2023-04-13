#ifndef PREPROCESSORGRIDTYPEDATAITEM_TOOLBARWIDGETCONTROLLER_H
#define PREPROCESSORGRIDTYPEDATAITEM_TOOLBARWIDGETCONTROLLER_H

#include <guicore/scalarstocolors/colormapsettingtoolbarwidgetcontroller.h>

#include "../preprocessorgridtypedataitem.h"

class PreProcessorGridTypeDataItem::ToolBarWidgetController : public ColorMapSettingToolBarWidgetController
{
	Q_OBJECT

public:
	ToolBarWidgetController(const std::string& name, ColorMapSettingToolBarWidgetI* widget, PreProcessorGridTypeDataItem* item);

private slots:
	void handleWidgetUpdate();

private:
	std::string m_name;
	PreProcessorGridTypeDataItem* m_item;
};

#endif // PREPROCESSORGRIDTYPEDATAITEM_TOOLBARWIDGETCONTROLLER_H
