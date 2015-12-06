#include "post2dwindownodevectorarrowdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <QMenu>
#include <QMouseEvent>
#include <QStandardItem>

Post2dWindowNodeVectorArrowDataItem::Post2dWindowNodeVectorArrowDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{}

Post2dWindowNodeVectorArrowDataItem::~Post2dWindowNodeVectorArrowDataItem()
{}

void Post2dWindowNodeVectorArrowDataItem::informSelection(VTKGraphicsView*)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->initNodeAttributeBrowser();
}

void Post2dWindowNodeVectorArrowDataItem::informDeselection(VTKGraphicsView*)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->clearNodeAttributeBrowser();
}

void Post2dWindowNodeVectorArrowDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->updateNodeAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeVectorArrowDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->fixNodeAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeVectorArrowDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* abAction = dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->showNodeAttributeBrowserAction();
	menu->addAction(abAction);
}

void Post2dWindowNodeVectorArrowDataItem::doLoadFromProjectMainFile(const QDomNode&)
{}

void Post2dWindowNodeVectorArrowDataItem::doSaveToProjectMainFile(QXmlStreamWriter&)
{}
