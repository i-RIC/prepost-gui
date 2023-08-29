#ifndef MEASUREDDATAPOINTGROUPDATAITEM_IMPL_H
#define MEASUREDDATAPOINTGROUPDATAITEM_IMPL_H

#include "../measureddatapointgroupdataitem.h"
#include "measureddatapointgroupdataitem_setting.h"

#include <unordered_map>

class vtkActor;
class vtkActor2D;
class ColorMapSettingContainer;
class ColorMapSettingToolBarWidget;

class MeasuredDataPointGroupDataItem::Impl
{
public:
	Impl(MeasuredDataPointGroupDataItem* item);
	~Impl();

	void setupActors();
	void updateCheckState();

	Setting m_setting;

	vtkActor* m_actor;
	vtkActor2D* m_legendActor;

	std::unordered_map<std::string, ColorMapSettingContainerI*> m_colorMapSettings;
	ToolBarWidget* m_toolBarWidget;

	ColorMapSettingToolBarWidget* m_colorMapToolBarWidget;
	ToolBarWidgetController* m_toolBarWidgetController;

private:
	MeasuredDataPointGroupDataItem* m_item;
};

#endif // MEASUREDDATAPOINTGROUPDATAITEM_IMPL_H
