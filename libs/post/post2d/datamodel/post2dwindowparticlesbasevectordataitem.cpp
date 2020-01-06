#include "post2dwindowparticlesbasetopdataitem.h"
#include "post2dwindowparticlesbasevectordataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/datamodel/vtkgraphicsview.h>

#include <QMenu>
#include <QMouseEvent>

Post2dWindowParticlesBaseVectorDataItem::Post2dWindowParticlesBaseVectorDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{}

void Post2dWindowParticlesBaseVectorDataItem::informSelection(VTKGraphicsView*)
{
	auto topItem = dynamic_cast<Post2dWindowParticlesBaseTopDataItem*> (parent()->parent());
	topItem->zoneDataItem()->initParticleResultAttributeBrowser(topItem->particleData());
}

void Post2dWindowParticlesBaseVectorDataItem::informDeselection(VTKGraphicsView*)
{
	auto topItem = dynamic_cast<Post2dWindowParticlesBaseTopDataItem*> (parent()->parent());
	topItem->zoneDataItem()->clearParticleResultAttributeBrowser();
}

void Post2dWindowParticlesBaseVectorDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
	auto topItem = dynamic_cast<Post2dWindowParticlesBaseTopDataItem*> (parent()->parent());
	topItem->zoneDataItem()->updateParticleResultAttributeBrowser(event->pos(), v);
}

void Post2dWindowParticlesBaseVectorDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void Post2dWindowParticlesBaseVectorDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
	auto topItem = dynamic_cast<Post2dWindowParticlesBaseTopDataItem*> (parent()->parent());
	topItem->zoneDataItem()->fixParticleResultAttributeBrowser(event->pos(), v);
}

void Post2dWindowParticlesBaseVectorDataItem::addCustomMenuItems(QMenu* menu)
{
	auto topItem = dynamic_cast<Post2dWindowParticlesBaseTopDataItem*> (parent()->parent());
	menu->addAction(topItem->showAttributeBrowserAction());
}
