#ifndef POST3DWINDOWCONTOURGROUPDATAITEM_IMPL_H
#define POST3DWINDOWCONTOURGROUPDATAITEM_IMPL_H

#include "../post3dwindowcontourgroupdataitem.h"
#include "post3dwindowcontourgroupdataitem_setting.h"

#include <guicore/scalarstocolors/colormapsettingcontainer.h>

class ColorMapSettingToolBarWidget;
class OpacityContainerWidget;

class vtkActor2D;

class Post3dWindowContourGroupDataItem::Impl
{
public:
	Impl(Post3dWindowContourGroupDataItem* item);
	~Impl();

	std::string m_target;

	ColorMapSettingContainer m_colorMapSetting;
	Setting m_setting;
	vtkActor2D* m_legendActor;

	ColorMapSettingToolBarWidget* m_colorMapToolBarWidget;
	OpacityContainerWidget* m_opacityToolBarWidget;
};

#endif // POST3DWINDOWCONTOURGROUPDATAITEM_IMPL_H
