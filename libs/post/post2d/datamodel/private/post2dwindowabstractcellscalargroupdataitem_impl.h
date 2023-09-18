#ifndef POST2DWINDOWABSTRACTCELLSCALARGROUPDATAITEM_IMPL_H
#define POST2DWINDOWABSTRACTCELLSCALARGROUPDATAITEM_IMPL_H

#include "../post2dwindowabstractcellscalargroupdataitem.h"
#include "post2dwindowabstractcellscalargroupdataitem_setting.h"

class ColorMapSettingToolBarWidget;
class OpacityContainerWidget;

class Post2dWindowAbstractCellScalarGroupDataItem::Impl
{
public:
	Impl(Post2dWindowAbstractCellScalarGroupDataItem* item);
	~Impl();

	std::string m_target;
	Setting m_setting;

	vtkActor* m_actor;
	vtkActor2D* m_legendActor;
	ColorMapSettingToolBarWidget* m_colorMapToolBarWidget;
	OpacityContainerWidget* m_opacityToolBarWidget;
};

#endif // POST2DWINDOWABSTRACTCELLSCALARGROUPDATAITEM_IMPL_H
