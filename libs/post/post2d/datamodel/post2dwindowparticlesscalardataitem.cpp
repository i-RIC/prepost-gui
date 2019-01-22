#include "post2dwindowparticlesscalardataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/datamodel/vtkgraphicsview.h>

#include <QMenu>
#include <QMouseEvent>

Post2dWindowParticlesScalarDataItem::Post2dWindowParticlesScalarDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{}

Post2dWindowParticlesScalarDataItem::~Post2dWindowParticlesScalarDataItem()
{}

void Post2dWindowParticlesScalarDataItem::informSelection(VTKGraphicsView*)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent()->parent())->initParticleResultAttributeBrowser();
}

void Post2dWindowParticlesScalarDataItem::informDeselection(VTKGraphicsView*)
{
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent()->parent())->clearParticleResultAttributeBrowser();
}

void Post2dWindowParticlesScalarDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent()->parent())->updateParticleResultAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowParticlesScalarDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void Post2dWindowParticlesScalarDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
	dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent()->parent())->fixParticleResultAttributeBrowser(QPoint(event->x(), event->y()), v);
}

void Post2dWindowParticlesScalarDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* abAction = dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent()->parent())->showAttributeBrowserActionForParticleResult();
	menu->addAction(abAction);
}
