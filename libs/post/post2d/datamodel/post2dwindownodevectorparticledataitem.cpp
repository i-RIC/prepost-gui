#include "../post2dwindowattributebrowsercontroller.h"
#include "post2dwindowcalculationresultdataitem.h"
#include "post2dwindownodescalargrouptopdataitem.h"
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

void Post2dWindowNodeVectorParticleDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	groupDataItem()->resultDataItem()->nodeScalarGroupTopDataItem()->attributeBrowserController()->initialize();
}

void Post2dWindowNodeVectorParticleDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	groupDataItem()->resultDataItem()->nodeScalarGroupTopDataItem()->attributeBrowserController()->clear();
}

void Post2dWindowNodeVectorParticleDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mousePressEvent(event, v);
}

void Post2dWindowNodeVectorParticleDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->resultDataItem()->nodeScalarGroupTopDataItem()->attributeBrowserController()->fix(event->pos(), v);
}

void Post2dWindowNodeVectorParticleDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->resultDataItem()->nodeScalarGroupTopDataItem()->attributeBrowserController()->update(event->pos(), v);
}

void Post2dWindowNodeVectorParticleDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(groupDataItem()->resultDataItem()->nodeScalarGroupTopDataItem()->showAttributeBrowserAction());
}

void Post2dWindowNodeVectorParticleDataItem::showPropertyDialog()
{
	groupDataItem()->showPropertyDialog();
}

QDialog* Post2dWindowNodeVectorParticleDataItem::propertyDialog(QWidget* parent)
{
	return groupDataItem()->propertyDialog(parent);
}

Post2dWindowZoneDataItem* Post2dWindowNodeVectorParticleDataItem::zoneDataItem() const
{
	return dynamic_cast<Post2dWindowZoneDataItem*> (parent()->parent());
}

Post2dWindowNodeVectorParticleGroupDataItem* Post2dWindowNodeVectorParticleDataItem::groupDataItem() const
{
	return dynamic_cast<Post2dWindowNodeVectorParticleGroupDataItem*> (parent());
}
