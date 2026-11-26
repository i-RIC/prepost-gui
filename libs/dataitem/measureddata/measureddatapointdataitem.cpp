#include <QStandardItem>
#include "measureddatapointdataitem.h"
#include "measureddatapointgroupdataitem.h"

#include <guicore/scalarstocolors/colormapsettingcontainer.h>

MeasuredDataPointDataItem::MeasuredDataPointDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{
	if (m_standardItem != nullptr) {
		m_standardItem->setToolTip(tr("Measured scalar values used to color the observation points."));
		if (m_standardItemCopy != nullptr) {m_standardItemCopy->setToolTip(m_standardItem->toolTip());}
	}
}

void MeasuredDataPointDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	auto s = activeColorMapSettingWithVisibleLegend();
	if (s == nullptr) {return;}

	s->legendSetting()->imgSetting()->controller()->handleResize(event, v);
}

void MeasuredDataPointDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeColorMapSettingWithVisibleLegend();
	if (s == nullptr) {return;}

	s->legendSetting()->imgSetting()->controller()->handleMouseMoveEvent(this, event, v);
}

void MeasuredDataPointDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeColorMapSettingWithVisibleLegend();
	if (s == nullptr) {return;}

	s->legendSetting()->imgSetting()->controller()->handleMousePressEvent(this, event, v);
}

void MeasuredDataPointDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = activeColorMapSettingWithVisibleLegend();
	if (s == nullptr) {return;}

	s->legendSetting()->imgSetting()->controller()->handleMouseReleaseEvent(this, event, v);
}

QDialog* MeasuredDataPointDataItem::propertyDialog(QWidget* parent)
{
	return groupDataItem()->propertyDialog(parent);
}

bool MeasuredDataPointDataItem::addToolBarButtons(QToolBar* toolBar)
{
	return groupDataItem()->addToolBarButtons(toolBar);
}

void MeasuredDataPointDataItem::showPropertyDialog()
{
	groupDataItem()->showPropertyDialog();
}

MeasuredDataPointGroupDataItem* MeasuredDataPointDataItem::groupDataItem() const
{
	return dynamic_cast<MeasuredDataPointGroupDataItem*>(parent());
}

ColorMapSettingContainerI* MeasuredDataPointDataItem::activeColorMapSettingWithVisibleLegend() const
{
	return groupDataItem()->activeColorMapSettingWithVisibleLegend();
}
