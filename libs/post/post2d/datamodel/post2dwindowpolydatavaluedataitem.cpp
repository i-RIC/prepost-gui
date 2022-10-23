#include "post2dwindowpolydatagroupdataitem.h"
#include "post2dwindowpolydatavaluedataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/scalarstocolors/colormapsettingcontainer.h>

#include <QMenu>

Post2dWindowPolyDataValueDataItem::Post2dWindowPolyDataValueDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{}

Post2dWindowPolyDataValueDataItem::~Post2dWindowPolyDataValueDataItem()
{}

void Post2dWindowPolyDataValueDataItem::informSelection(VTKGraphicsView* v)
{
	auto s = polyDataGroupDataItem()->activeSetting();
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleSelection(v);
	zoneDataItem()->initPolyDataResultAttributeBrowser();
}

void Post2dWindowPolyDataValueDataItem::informDeselection(VTKGraphicsView* v)
{
	auto s = polyDataGroupDataItem()->activeSetting();
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleDeselection(v);
	zoneDataItem()->clearPolyDataResultAttributeBrowser();
}

void Post2dWindowPolyDataValueDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto gdi = polyDataGroupDataItem();
	auto s = gdi->activeSetting();
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleMouseMoveEvent(event, v);
	zoneDataItem()->updatePolyDataResultAttributeBrowser(gdi->name(), event->pos(), v);
}

void Post2dWindowPolyDataValueDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = polyDataGroupDataItem()->activeSetting();
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleMousePressEvent(event, v);
}

void Post2dWindowPolyDataValueDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto gdi = polyDataGroupDataItem();
	auto s = gdi->activeSetting();
	if (s == nullptr) {return;}

	s->legend.imageSetting.controller()->handleMouseReleaseEvent(event, v);
	zoneDataItem()->fixPolyDataResultAttributeBrowser(gdi->name(), event->pos(), v);
}

void Post2dWindowPolyDataValueDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* a = zoneDataItem()->showAttributeBrowserActionForPolyDataResult();
	menu->addAction(a);
}

Post2dWindowZoneDataItem* Post2dWindowPolyDataValueDataItem::zoneDataItem() const
{
	return dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent()->parent());
}


Post2dWindowPolyDataGroupDataItem* Post2dWindowPolyDataValueDataItem::polyDataGroupDataItem() const
{
	return dynamic_cast<Post2dWindowPolyDataGroupDataItem*>(parent());
}
