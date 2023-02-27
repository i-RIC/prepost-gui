#ifndef MEASUREDDATAVECTORGROUPDATAITEM_IMPL_H
#define MEASUREDDATAVECTORGROUPDATAITEM_IMPL_H

#include "../measureddatavectorgroupdataitem.h"
#include "../measureddatavectorsetting.h"

#include <unordered_map>

class vtkActor;
class vtkActor2D;

class MeasuredDataVectorGroupDataItem::Impl
{
public:
	Impl(MeasuredDataVectorGroupDataItem* item);
	~Impl();

	void setupActors();
	void updateCheckState();
	void updateActorSettings();

	vtkActor* m_actor;
	vtkActor2D* m_legendActor;

	MeasuredDataVectorSetting m_setting;

private:
	MeasuredDataVectorGroupDataItem* m_item;
};

#endif // MEASUREDDATAVECTORGROUPDATAITEM_IMPL_H
