#include "post2dwindowparticlesvectordataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/datamodel/vtkgraphicsview.h>

#include <QMenu>
#include <QMouseEvent>

Post2dWindowParticlesVectorDataItem::Post2dWindowParticlesVectorDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{}

void Post2dWindowParticlesVectorDataItem::informSelection(VTKGraphicsView*)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent()->parent())->initParticleResultAttributeBrowser();
}

void Post2dWindowParticlesVectorDataItem::informDeselection(VTKGraphicsView*)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent()->parent())->clearParticleResultAttributeBrowser();
}

void Post2dWindowParticlesVectorDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent()->parent())->updateParticleResultAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowParticlesVectorDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void Post2dWindowParticlesVectorDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent()->parent())->fixParticleResultAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowParticlesVectorDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* abAction = dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent()->parent())->showAttributeBrowserActionForParticleResult();
	menu->addAction(abAction);
}
