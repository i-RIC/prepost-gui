#include "post2dwindownodescalardataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/datamodel/vtkgraphicsview.h>

#include <QMenu>
#include <QMouseEvent>

Post2dWindowNodeScalarDataItem::Post2dWindowNodeScalarDataItem(const std::string& name, const QString& caption, Post2dWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{}

Post2dWindowNodeScalarDataItem::~Post2dWindowNodeScalarDataItem()
{}

void Post2dWindowNodeScalarDataItem::informSelection(VTKGraphicsView*)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->initNodeAttributeBrowser();
}

void Post2dWindowNodeScalarDataItem::informDeselection(VTKGraphicsView*)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->clearNodeAttributeBrowser();
}

void Post2dWindowNodeScalarDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->updateNodeAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeScalarDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void Post2dWindowNodeScalarDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->fixNodeAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeScalarDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* abAction = dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->showNodeAttributeBrowserAction();
	menu->addAction(abAction);
}
