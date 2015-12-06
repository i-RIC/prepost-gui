#include "post2dwindownodevectorstreamlinedataitem.h"
#include "post2dwindownodevectorstreamlinegroupdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>

#include <QAction>
#include <QMenu>
#include <QMouseEvent>
#include <QStandardItem>

#include <vtkStructuredGrid.h>

Post2dWindowNodeVectorStreamlineDataItem::Post2dWindowNodeVectorStreamlineDataItem(const std::string& name, const QString& caption, Post2dWindowDataItem* parent) :
	Post2dWindowDataItem {caption, QIcon(":/libs/guibase/images/iconPaper.png"), parent},
	m_name (name)
{
	setupStandardItem(NotChecked, NotReorderable, NotDeletable);

	connect(this, SIGNAL(changed(Post2dWindowNodeVectorStreamlineDataItem*)),
					parent, SLOT(exclusivelyCheck(Post2dWindowNodeVectorStreamlineDataItem*)));
}

const std::string& Post2dWindowNodeVectorStreamlineDataItem::name() const
{
	return m_name;
}

void Post2dWindowNodeVectorStreamlineDataItem::handleStandardItemChange()
{
	emit changed(this);
	setModified();
}

void Post2dWindowNodeVectorStreamlineDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->initNodeAttributeBrowser();
}

void Post2dWindowNodeVectorStreamlineDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->clearNodeAttributeBrowser();
}

void Post2dWindowNodeVectorStreamlineDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->updateNodeAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeVectorStreamlineDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->fixNodeAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeVectorStreamlineDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* abAction = dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->showNodeAttributeBrowserAction();
	menu->addAction(abAction);
}

void Post2dWindowNodeVectorStreamlineDataItem::doLoadFromProjectMainFile(const QDomNode&)
{}

void Post2dWindowNodeVectorStreamlineDataItem::doSaveToProjectMainFile(QXmlStreamWriter&)
{}
