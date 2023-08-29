#ifndef MEASUREDDATAVECTORGROUPDATAITEM_IMPL_H
#define MEASUREDDATAVECTORGROUPDATAITEM_IMPL_H

#include "../measureddatavectorgroupdataitem.h"
#include "measureddatavectorgroupdataitem_setting.h"

#include <unordered_map>

class ArrowsSettingToolBarWidget;

class vtkActor;
class vtkActor2D;

class MeasuredDataVectorGroupDataItem::Impl
{
public:
	Impl(MeasuredDataVectorGroupDataItem* item);
	~Impl();

	void setupActors();
	void updateCheckState();

	vtkActor* m_actor;
	vtkActor2D* m_legendActor;
	ArrowsSettingToolBarWidget* m_arrowsToolBarWidget;

	Setting m_setting;

private:
	MeasuredDataVectorGroupDataItem* m_item;
};

#endif // MEASUREDDATAVECTORGROUPDATAITEM_IMPL_H
