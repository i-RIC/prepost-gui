#include "post2dwindownodevectorarrowdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <QMenu>
#include <QMouseEvent>
#include <QStandardItem>

Post2dWindowNodeVectorArrowDataItem::Post2dWindowNodeVectorArrowDataItem(const QString& name, const QString& caption, Post2dWindowDataItem* parent) :
	Post2dWindowDataItem {caption, QIcon(":/libs/guibase/images/iconPaper.png"), parent},
	m_name {name}
{
	setupStandardItem(NotChecked, NotReorderable, NotDeletable);

	connect(this, SIGNAL(changed(Post2dWindowNodeVectorArrowDataItem*)),
					parent, SLOT(exclusivelyCheck(Post2dWindowNodeVectorArrowDataItem*)));
}

void Post2dWindowNodeVectorArrowDataItem::handleStandardItemChange()
{
	emit changed(this);
	setModified();
}

void Post2dWindowNodeVectorArrowDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->initNodeAttributeBrowser();
}

void Post2dWindowNodeVectorArrowDataItem::informDeselection(VTKGraphicsView* /*v*/)
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
