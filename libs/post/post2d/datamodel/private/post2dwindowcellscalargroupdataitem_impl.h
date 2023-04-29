#ifndef POST2DWINDOWCELLSCALARGROUPDATAITEM_IMPL_H
#define POST2DWINDOWCELLSCALARGROUPDATAITEM_IMPL_H

#include "../post2dwindowcellscalargroupdataitem.h"
#include "post2dwindowcellscalargroupdataitem_setting.h"

class ColorMapSettingToolBarWidget;
class OpacityContainerWidget;

class Post2dWindowCellScalarGroupDataItem::Impl
{
public:
	Impl(Post2dWindowCellScalarGroupDataItem* item);
	~Impl();

	std::string m_target;
	Setting m_setting;

	vtkActor* m_actor;
	vtkActor2D* m_legendActor;
	ColorMapSettingToolBarWidget* m_colorMapToolBarWidget;
	OpacityContainerWidget* m_opacityToolBarWidget;
};

#endif // POST2DWINDOWCELLSCALARGROUPDATAITEM_IMPL_H
