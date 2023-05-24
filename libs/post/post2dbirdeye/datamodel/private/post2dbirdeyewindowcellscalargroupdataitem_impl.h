#ifndef POST2DBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_IMPL_H
#define POST2DBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_IMPL_H

#include "../post2dbirdeyewindowcellscalargroupdataitem.h"
#include "post2dbirdeyewindowcellscalargroupdataitem_setting.h"

#include <unordered_map>

class ColorMapSettingToolBarWidget;
class OpacityContainerWidget;
class ValueRangeContainer;

class Post2dBirdEyeWindowCellScalarGroupDataItem::Impl
{
public:
	Impl(const std::string& elevationTarget, Post2dBirdEyeWindowCellScalarGroupDataItem* item);
	~Impl();

	void createOrUpdateColorMapsSetting(const std::string& name, const QString& caption, const ValueRangeContainer& range);

	std::string m_elevationTarget;
	Setting m_setting;

	vtkActor* m_actor;
	vtkActor2D* m_legendActor;
	std::unordered_map<std::string, ColorMapSettingContainer*> m_colorMapSettings;
	ColorMapSettingToolBarWidget* m_colorMapToolBarWidget;
	OpacityContainerWidget* m_opacityToolBarWidget;
};

#endif // POST2DBIRDEYEWINDOWCELLSCALARGROUPDATAITEM_IMPL_H
