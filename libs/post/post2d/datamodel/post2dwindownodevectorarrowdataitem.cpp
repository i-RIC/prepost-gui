#include "post2dwindownodevectorarrowdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/datamodel/vtkgraphicsview.h>

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
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->initNodeResultAttributeBrowser();
}

void Post2dWindowNodeVectorArrowDataItem::informDeselection(VTKGraphicsView*)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->clearNodeResultAttributeBrowser();
}

void Post2dWindowNodeVectorArrowDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->updateNodeResultAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeVectorArrowDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void Post2dWindowNodeVectorArrowDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->fixNodeResultAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeVectorArrowDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* abAction = dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->showAttributeBrowserActionForNodeResult();
	menu->addAction(abAction);
}
