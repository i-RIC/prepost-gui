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

	s->legendSetting()->imgSetting()->controller()->handleSelection(v);
}

void MeasuredDataVectorDataItem::informDeselection(VTKGraphicsView* v)
{
	auto& as = setting().arrowsSetting;
	as.legend.imageSetting.controller()->handleDeselection(v);

	if (as.colorMode == ArrowsSettingContainer::ColorMode::Custom) {return;}

	auto s = pointGroupDataItem()->colorMapSetting(iRIC::toStr(as.colorTarget));
	if (s == nullptr) {return;}

	s->legendSetting()->imgSetting()->controller()->handleDeselection(v);
}

void MeasuredDataVectorDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	auto& as = setting().arrowsSetting;
	as.legend.imageSetting.controller()->handleResize(event, v);

	if (as.colorMode == ArrowsSettingContainer::ColorMode::Custom) {return;}

	auto s = pointGroupDataItem()->colorMapSetting(iRIC::toStr(as.colorTarget));
	if (s == nullptr) {return;}

	s->legendSetting()->imgSetting()->controller()->handleResize(event, v);
}

void MeasuredDataVectorDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto& as = setting().arrowsSetting;
	as.legend.imageSetting.controller()->handleMouseMoveEvent(event, v);

	if (as.colorMode == ArrowsSettingContainer::ColorMode::Custom) {return;}

	auto s = pointGroupDataItem()->colorMapSetting(iRIC::toStr(as.colorTarget));
	if (s == nullptr) {return;}

	s->legendSetting()->imgSetting()->controller()->handleMouseMoveEvent(event, v);
}

void MeasuredDataVectorDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto& as = setting().arrowsSetting;
	as.legend.imageSetting.controller()->handleMousePressEvent(event, v);

	if (as.colorMode == ArrowsSettingContainer::ColorMode::Custom) {return;}

	auto s = pointGroupDataItem()->colorMapSetting(iRIC::toStr(as.colorTarget));
	if (s == nullptr) {return;}

	s->legendSetting()->imgSetting()->controller()->handleMousePressEvent(event, v);
}

void MeasuredDataVectorDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto& as = setting().arrowsSetting;
	as.legend.imageSetting.controller()->handleMouseReleaseEvent(event, v);

	if (as.colorMode == ArrowsSettingContainer::ColorMode::Custom) {return;}

	auto s = pointGroupDataItem()->colorMapSetting(iRIC::toStr(as.colorTarget));
	if (s == nullptr) {return;}

	s->legendSetting()->imgSetting()->controller()->handleMouseReleaseEvent(event, v);
}

QDialog* MeasuredDataVectorDataItem::propertyDialog(QWidget* parent)
{
	return groupDataItem()->propertyDialog(parent);
}

void MeasuredDataVectorDataItem::showPropertyDialog()
{
	groupDataItem()->showPropertyDialog();
}

MeasuredDataPointGroupDataItem* MeasuredDataVectorDataItem::pointGroupDataItem() const
{
	return dynamic_cast<MeasuredDataFileDataItem*> (parent()->parent())->pointGroupDataItem();
}

MeasuredDataVectorGroupDataItem* MeasuredDataVectorDataItem::groupDataItem() const
{
	return dynamic_cast<MeasuredDataVectorGroupDataItem*>(parent());
}

MeasuredDataVectorGroupDataItem::Setting& MeasuredDataVectorDataItem::setting()
{
	return groupDataItem()->impl->m_setting;
}
