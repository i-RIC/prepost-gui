#ifndef GRIDBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_IMPL_H
#define GRIDBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_IMPL_H

#include "../gridbirdeyewindowcellscalargroupdataitem.h"
#include "gridbirdeyewindowcellscalargroupdataitem_setting.h"

#include <unordered_map>

class ColorMapSettingToolBarWidget;
class OpacityContainerWidget;

class GridBirdEyeWindowCellScalarGroupDataItem::Impl
{
public:
	Impl(const std::string& elevationTarget, GridBirdEyeWindowCellScalarGroupDataItem* item);
	~Impl();

	std::string m_elevationTarget;
	Setting m_setting;

	vtkActor* m_actor;
	vtkActor2D* m_legendActor;
	std::unordered_map<std::string, ColorMapSettingContainerI*> m_colorMapSettings;
	ColorMapSettingToolBarWidget* m_colorMapToolBarWidget;
	OpacityContainerWidget* m_opacityToolBarWidget;
};

#endif // GRIDBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_IMPL_H
