#include "measureddatapointdataitem.h"
#include "measureddatapointgroupdataitem.h"

#include <guicore/scalarstocolors/colormapsettingcontainer.h>

MeasuredDataPointDataItem::MeasuredDataPointDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{}

void MeasuredDataPointDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s == nullptr) {return;}

	s->legendSetting()->imgSetting()->controller()->handleResize(event, v);
}

void MeasuredDataPointDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s == nullptr) {return;}

	s->legendSetting()->imgSetting()->controller()->handleMouseMoveEvent(event, v);
}

void MeasuredDataPointDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s == nullptr) {return;}

	s->legendSetting()->imgSetting()->controller()->handleMousePressEvent(event, v);
}

void MeasuredDataPointDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s == nullptr) {return;}

	s->legendSetting()->imgSetting()->controller()->handleMouseReleaseEvent(event, v);
}

QDialog* MeasuredDataPointDataItem::propertyDialog(QWidget* parent)
{
	return groupDataItem()->propertyDialog(parent);
}

void MeasuredDataPointDataItem::showPropertyDialog()
{
	groupDataItem()->showPropertyDialog();
}

MeasuredDataPointGroupDataItem* MeasuredDataPointDataItem::groupDataItem() const
{
	return dynamic_cast<MeasuredDataPointGroupDataItem*>(parent());
}

ColorMapSettingContainerI* MeasuredDataPointDataItem::activeSetting() const
{
	return groupDataItem()->activeSetting();
}
