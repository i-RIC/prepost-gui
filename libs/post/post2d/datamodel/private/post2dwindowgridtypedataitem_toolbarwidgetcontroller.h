#ifndef POST2DWINDOWGRIDTYPEDATAITEM_TOOLBARWIDGETCONTROLLER_H
#define POST2DWINDOWGRIDTYPEDATAITEM_TOOLBARWIDGETCONTROLLER_H

#include <guicore/scalarstocolors/colormapsettingtoolbarwidgetcontroller.h>

#include "../post2dwindowgridtypedataitem.h"

class Post2dWindowGridTypeDataItem::ToolBarWidgetController : public ColorMapSettingToolBarWidgetController
{
public:
	ToolBarWidgetController(const std::string& name, ColorMapSettingToolBarWidget* widget, Post2dWindowGridTypeDataItem* item);

private slots:
	void handleWidgetUpdate() override;

private:
	std::string m_name;
	Post2dWindowGridTypeDataItem* m_item;
};

#endif // POST2DWINDOWGRIDTYPEDATAITEM_TOOLBARWIDGETCONTROLLER_H
