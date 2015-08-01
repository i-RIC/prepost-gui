#include "post2dwindownodevectorparticledataitem.h"
#include "post2dwindownodevectorparticlegroupdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>

#include <QAction>
#include <QMenu>
#include <QMouseEvent>
#include <QStandardItem>

#include <vtkStructuredGrid.h>

Post2dWindowNodeVectorParticleDataItem::Post2dWindowNodeVectorParticleDataItem(const QString& name, const QString& caption, Post2dWindowDataItem* parent) :
	Post2dWindowDataItem {caption, QIcon(":/libs/guibase/images/iconPaper.png"), parent},
	m_name {name}
{
	setupStandardItem(NotChecked, NotReorderable, NotDeletable);

	connect(this, SIGNAL(changed(Post2dWindowNodeVectorParticleDataItem*)),
					parent, SLOT(exclusivelyCheck(Post2dWindowNodeVectorParticleDataItem*)));
}

void Post2dWindowNodeVectorParticleDataItem::handleStandardItemChange()
{
	emit changed(this);
	setModified();
}

void Post2dWindowNodeVectorParticleDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->initNodeAttributeBrowser();
}

void Post2dWindowNodeVectorParticleDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->clearNodeAttributeBrowser();
}

void Post2dWindowNodeVectorParticleDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->updateNodeAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeVectorParticleDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->fixNodeAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeVectorParticleDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* abAction = dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->showNodeAttributeBrowserAction();
	menu->addAction(abAction);
}
