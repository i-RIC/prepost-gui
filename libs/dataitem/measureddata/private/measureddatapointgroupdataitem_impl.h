#ifndef MEASUREDDATAPOINTGROUPDATAITEM_IMPL_H
#define MEASUREDDATAPOINTGROUPDATAITEM_IMPL_H

#include "../measureddatapointgroupdataitem.h"
#include "../measureddatapointsetting.h"

#include <unordered_map>

class vtkActor;
class vtkActor2D;
class ColorMapSettingContainer;

class MeasuredDataPointGroupDataItem::Impl
{
public:
	Impl(MeasuredDataPointGroupDataItem* item);
	~Impl();

	void setupActors();
	void updateCheckState();
	void updateActorSettings();

	MeasuredDataPointSetting m_setting;

	vtkActor* m_actor;
	vtkActor2D* m_legendActor;

	std::unordered_map<std::string, ColorMapSettingContainer*> m_colorMapSettings;

private:
	MeasuredDataPointGroupDataItem* m_item;
};

#endif // MEASUREDDATAPOINTGROUPDATAITEM_IMPL_H
