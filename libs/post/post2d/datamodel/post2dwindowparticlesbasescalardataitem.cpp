#include "post2dwindowparticlesbasescalardataitem.h"
#include "post2dwindowparticlesbasetopdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/datamodel/vtkgraphicsview.h>

#include <QMenu>
#include <QMouseEvent>

Post2dWindowParticlesBaseScalarDataItem::Post2dWindowParticlesBaseScalarDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{}

Post2dWindowParticlesBaseScalarDataItem::~Post2dWindowParticlesBaseScalarDataItem()
{}

void Post2dWindowParticlesBaseScalarDataItem::informSelection(VTKGraphicsView*)
{
	auto topItem = dynamic_cast<Post2dWindowParticlesBaseTopDataItem*> (parent()->parent());
	topItem->zoneDataItem()->initParticleResultAttributeBrowser(topItem->particleData());
}

void Post2dWindowParticlesBaseScalarDataItem::informDeselection(VTKGraphicsView*)
{
	auto topItem = dynamic_cast<Post2dWindowParticlesBaseTopDataItem*> (parent()->parent());
	topItem->zoneDataItem()->clearParticleResultAttributeBrowser();
}

void Post2dWindowParticlesBaseScalarDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
	auto topItem = dynamic_cast<Post2dWindowParticlesBaseTopDataItem*> (parent()->parent());
	topItem->zoneDataItem()->updateParticleResultAttributeBrowser(event->pos(), v);
}

void Post2dWindowParticlesBaseScalarDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void Post2dWindowParticlesBaseScalarDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
	auto topItem = dynamic_cast<Post2dWindowParticlesBaseTopDataItem*> (parent()->parent());
	topItem->zoneDataItem()->fixParticleResultAttributeBrowser(event->pos(), v);
}

void Post2dWindowParticlesBaseScalarDataItem::addCustomMenuItems(QMenu* menu)
{
	auto topItem = dynamic_cast<Post2dWindowParticlesBaseTopDataItem*> (parent()->parent());
	menu->addAction(topItem->showAttributeBrowserAction());
}
