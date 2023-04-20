#include "measureddatapointdataitem.h"
#include "measureddatapointgroupdataitem.h"

#include <guicore/scalarstocolors/colormapsettingcontainer.h>

MeasuredDataPointDataItem::MeasuredDataPointDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{}

void MeasuredDataPointDataItem::informSelection(VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleSelection(v);
}

void MeasuredDataPointDataItem::informDeselection(VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleDeselection(v);
}

void MeasuredDataPointDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleResize(event, v);
}

void MeasuredDataPointDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleMouseMoveEvent(event, v);
}

void MeasuredDataPointDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleMousePressEvent(event, v);
}

void MeasuredDataPointDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeSetting();
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleMouseReleaseEvent(event, v);
}

ColorMapSettingContainer* MeasuredDataPointDataItem::activeSetting() const
{
	return dynamic_cast<MeasuredDataPointGroupDataItem*>(parent())->activeSetting();
}
