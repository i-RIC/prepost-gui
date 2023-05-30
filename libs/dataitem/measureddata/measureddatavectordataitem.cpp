#include "measureddatafiledataitem.h"
#include "measureddatapointgroupdataitem.h"
#include "measureddatavectordataitem.h"
#include "measureddatavectorgroupdataitem.h"
#include "private/measureddatavectorgroupdataitem_impl.h"
#include "private/measureddatavectorgroupdataitem_setting.h"

#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <misc/stringtool.h>

#include <QStandardItem>

MeasuredDataVectorDataItem::MeasuredDataVectorDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem {name, caption, parent}
{}

void MeasuredDataVectorDataItem::informSelection(VTKGraphicsView* v)
{
	auto& as = setting().arrowsSetting;
	as.legend.imageSetting.controller()->handleSelection(v);

	if (as.colorMode == ArrowsSettingContainer::ColorMode::Custom) {return;}

	auto s = pointGroupDataItem()->colorMapSetting(iRIC::toStr(as.colorTarget));
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleSelection(v);
}

void MeasuredDataVectorDataItem::informDeselection(VTKGraphicsView* v)
{
	auto& as = setting().arrowsSetting;
	as.legend.imageSetting.controller()->handleDeselection(v);

	if (as.colorMode == ArrowsSettingContainer::ColorMode::Custom) {return;}

	auto s = pointGroupDataItem()->colorMapSetting(iRIC::toStr(as.colorTarget));
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleDeselection(v);
}

void MeasuredDataVectorDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	auto& as = setting().arrowsSetting;
	as.legend.imageSetting.controller()->handleResize(event, v);

	if (as.colorMode == ArrowsSettingContainer::ColorMode::Custom) {return;}

	auto s = pointGroupDataItem()->colorMapSetting(iRIC::toStr(as.colorTarget));
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleResize(event, v);
}

void MeasuredDataVectorDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto& as = setting().arrowsSetting;
	as.legend.imageSetting.controller()->handleMouseMoveEvent(event, v);

	if (as.colorMode == ArrowsSettingContainer::ColorMode::Custom) {return;}

	auto s = pointGroupDataItem()->colorMapSetting(iRIC::toStr(as.colorTarget));
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleMouseMoveEvent(event, v);
}

void MeasuredDataVectorDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto& as = setting().arrowsSetting;
	as.legend.imageSetting.controller()->handleMousePressEvent(event, v);

	if (as.colorMode == ArrowsSettingContainer::ColorMode::Custom) {return;}

	auto s = pointGroupDataItem()->colorMapSetting(iRIC::toStr(as.colorTarget));
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleMousePressEvent(event, v);
}

void MeasuredDataVectorDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto& as = setting().arrowsSetting;
	as.legend.imageSetting.controller()->handleMouseReleaseEvent(event, v);

	if (as.colorMode == ArrowsSettingContainer::ColorMode::Custom) {return;}

	auto s = pointGroupDataItem()->colorMapSetting(iRIC::toStr(as.colorTarget));
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleMouseReleaseEvent(event, v);
}

MeasuredDataPointGroupDataItem* MeasuredDataVectorDataItem::pointGroupDataItem() const
{
	return dynamic_cast<MeasuredDataFileDataItem*> (parent()->parent())->pointGroupDataItem();
}

MeasuredDataVectorGroupDataItem::Setting& MeasuredDataVectorDataItem::setting()
{
	return dynamic_cast<MeasuredDataVectorGroupDataItem*>(parent())->impl->m_setting;
}
