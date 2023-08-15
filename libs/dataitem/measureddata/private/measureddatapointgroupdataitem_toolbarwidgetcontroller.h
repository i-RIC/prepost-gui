#ifndef MEASUREDDATAPOINTGROUPDATAITEM_TOOLBARWIDGETCONTROLLER_H
#define MEASUREDDATAPOINTGROUPDATAITEM_TOOLBARWIDGETCONTROLLER_H

#include "../measureddatapointgroupdataitem.h"

#include <guicore/scalarstocolors/colormapsettingtoolbarwidgetcontroller.h>

class MeasuredDataPointGroupDataItem::ToolBarWidgetController : public ColorMapSettingToolBarWidgetController
{
public:
	ToolBarWidgetController(ColorMapSettingToolBarWidget* widget, MeasuredDataPointGroupDataItem* item);

private slots:
	void handleWidgetUpdate() override;

private:
	MeasuredDataPointGroupDataItem* m_item;
};

#endif // MEASUREDDATAPOINTGROUPDATAITEM_TOOLBARWIDGETCONTROLLER_H
