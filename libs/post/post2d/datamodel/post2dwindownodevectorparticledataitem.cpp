#include "post2dwindownodevectorparticledataitem.h"
#include "post2dwindownodevectorparticlegroupdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>

#include <QAction>
#include <QMenu>
#include <QMouseEvent>
#include <QStandardItem>

#include <vtkStructuredGrid.h>

Post2dWindowNodeVectorParticleDataItem::Post2dWindowNodeVectorParticleDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{}

Post2dWindowNodeVectorParticleDataItem::~Post2dWindowNodeVectorParticleDataItem()
{}

void Post2dWindowNodeVectorParticleDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->initNodeResultAttributeBrowser();
}

void Post2dWindowNodeVectorParticleDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->clearNodeResultAttributeBrowser();
}

void Post2dWindowNodeVectorParticleDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->updateNodeResultAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeVectorParticleDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->fixNodeResultAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowNodeVectorParticleDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* abAction = dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->showAttributeBrowserActionForNodeResult();
	menu->addAction(abAction);
}
