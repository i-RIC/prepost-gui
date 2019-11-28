#include "post2dwindowpolydatavaluedataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/datamodel/vtkgraphicsview.h>

#include <QMenu>

Post2dWindowPolyDataValueDataItem::Post2dWindowPolyDataValueDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{}

Post2dWindowPolyDataValueDataItem::~Post2dWindowPolyDataValueDataItem()
{}

void Post2dWindowPolyDataValueDataItem::informSelection(VTKGraphicsView* v)
{
	// @todo implement this
	// zoneDataItem()->initParticleBrowser();
}

void Post2dWindowPolyDataValueDataItem::informDeselection(VTKGraphicsView* v)
{
	// @todo implement this
	// zoneDataItem()->clearParticleBrowser();
}

void Post2dWindowPolyDataValueDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
}

void Post2dWindowPolyDataValueDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void Post2dWindowPolyDataValueDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
	// zoneDataItem()->fixParticleBrowser(event->pos(), v);
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
