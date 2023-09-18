#include "../post2dwindowattributebrowsercontroller.h"
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

void Post2dWindowPolyDataValueDataItem::showPropertyDialog()
{
	polyDataGroupDataItem()->showPropertyDialog();
}

QDialog* Post2dWindowPolyDataValueDataItem::propertyDialog(QWidget* parent)
{
	return polyDataGroupDataItem()->propertyDialog(parent);
}

void Post2dWindowPolyDataValueDataItem::informSelection(VTKGraphicsView* v)
{
	polyDataGroupDataItem()->attributeBrowserController()->initialize();
}

void Post2dWindowPolyDataValueDataItem::informDeselection(VTKGraphicsView* v)
{
	polyDataGroupDataItem()->attributeBrowserController()->clear();
}

void Post2dWindowPolyDataValueDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto gdi = polyDataGroupDataItem();
	auto s = gdi->activeSetting();
	if (s == nullptr) {return;}

	s->legendSetting()->imgSetting()->controller()->handleMouseMoveEvent(event, v);
	polyDataGroupDataItem()->attributeBrowserController()->update(event->pos(), v);
}

void Post2dWindowPolyDataValueDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto s = polyDataGroupDataItem()->activeSetting();
	if (s == nullptr) {return;}

	s->legendSetting()->imgSetting()->controller()->handleMousePressEvent(event, v);
}

void Post2dWindowPolyDataValueDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	auto gdi = polyDataGroupDataItem();
	auto s = gdi->activeSetting();
	if (s == nullptr) {return;}

	s->legendSetting()->imgSetting()->controller()->handleMouseReleaseEvent(event, v);
	polyDataGroupDataItem()->attributeBrowserController()->fix(event->pos(), v);
}

void Post2dWindowPolyDataValueDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(polyDataGroupDataItem()->showAttributeBrowserAction());
}

bool Post2dWindowPolyDataValueDataItem::addToolBarButtons(QToolBar* toolBar)
{
	auto gdi = polyDataGroupDataItem();
	return gdi->addToolBarButtons(toolBar);
}

Post2dWindowZoneDataItem* Post2dWindowPolyDataValueDataItem::zoneDataItem() const
{
	return polyDataGroupDataItem()->zoneDataItem();
}


Post2dWindowPolyDataGroupDataItem* Post2dWindowPolyDataValueDataItem::polyDataGroupDataItem() const
{
	return dynamic_cast<Post2dWindowPolyDataGroupDataItem*>(parent());
}
