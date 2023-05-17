#ifndef GRIDBIRDEYEWINDOWNODESCALARGROUPDATAITEM_IMPL_H
#define GRIDBIRDEYEWINDOWNODESCALARGROUPDATAITEM_IMPL_H

#include "../gridbirdeyewindownodescalargroupdataitem.h"
#include "gridbirdeyewindownodescalargroupdataitem_setting.h"

#include <unordered_map>

class ColorMapSettingToolBarWidget;
class OpacityContainerWidget;

class GridBirdEyeWindowNodeScalarGroupDataItem::Impl
{
public:
	Impl(const std::string& elevationTarget, GridBirdEyeWindowNodeScalarGroupDataItem* item);
	~Impl();

	std::string m_elevationTarget;
	Setting m_setting;

	vtkActor* m_actor;
	vtkActor2D* m_legendActor;
	std::unordered_map<std::string, ColorMapSettingContainerI*> m_colorMapSettings;
	ColorMapSettingToolBarWidget* m_colorMapToolBarWidget;
	OpacityContainerWidget* m_opacityToolBarWidget;
};

#endif // GRIDBIRDEYEWINDOWNODESCALARGROUPDATAITEM_IMPL_H
