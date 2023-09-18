#ifndef POST2DWINDOWNODESCALARGROUPDATAITEM_IMPL_H
#define POST2DWINDOWNODESCALARGROUPDATAITEM_IMPL_H

#include "../post2dwindownodescalargroupdataitem.h"
#include "post2dwindownodescalargroupdataitem_setting.h"

class ColorMapSettingToolBarWidget;
class OpacityContainerWidget;

class Post2dWindowNodeScalarGroupDataItem::Impl
{
public:
	Impl(Post2dWindowNodeScalarGroupDataItem* item);
	~Impl();

	std::string m_target;

	Setting m_setting;

	vtkActor* m_actor;
	vtkActor2D* m_legendActor;
	ColorMapSettingToolBarWidget* m_colorMapToolBarWidget;
	OpacityContainerWidget* m_opacityToolBarWidget;

	ShapeExporter* m_shapeExporter;
};

#endif // POST2DWINDOWNODESCALARGROUPDATAITEM_IMPL_H
