#ifndef PREPROCESSORGRIDTYPEDATAITEM_TOOLBARWIDGETCONTROLLER_H
#define PREPROCESSORGRIDTYPEDATAITEM_TOOLBARWIDGETCONTROLLER_H

#include "../preprocessorgridtypedataitem.h"

#include <guicore/scalarstocolors/colormapsettingtoolbarwidgetcontroller.h>

class PreProcessorGridTypeDataItem::ToolBarWidgetController : public ColorMapSettingToolBarWidgetController
{
	Q_OBJECT

public:
	ToolBarWidgetController(const std::string& name, ColorMapSettingToolBarWidget* widget, PreProcessorGridTypeDataItem* item);

private slots:
	void handleWidgetUpdate() override;

private:
	std::string m_name;
	PreProcessorGridTypeDataItem* m_item;
};

#endif // PREPROCESSORGRIDTYPEDATAITEM_TOOLBARWIDGETCONTROLLER_H
