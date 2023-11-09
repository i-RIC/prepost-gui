#include "../post2dwindowattributebrowsercontroller.h"
#include "post2dwindowcalculationresultdataitem.h"
#include "post2dwindownodescalargrouptopdataitem.h"
#include "post2dwindownodevectorstreamlinedataitem.h"
#include "post2dwindownodevectorstreamlinegroupdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <QMenu>

Post2dWindowNodeVectorStreamlineDataItem::Post2dWindowNodeVectorStreamlineDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{}

Post2dWindowNodeVectorStreamlineDataItem::~Post2dWindowNodeVectorStreamlineDataItem()
{}

void Post2dWindowNodeVectorStreamlineDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	groupDataItem()->resultDataItem()->nodeScalarGroupTopDataItem()->attributeBrowserController()->initialize();
}

void Post2dWindowNodeVectorStreamlineDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	groupDataItem()->resultDataItem()->nodeScalarGroupTopDataItem()->attributeBrowserController()->clear();
}

void Post2dWindowNodeVectorStreamlineDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mousePressEvent(event, v);
}

void Post2dWindowNodeVectorStreamlineDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->resultDataItem()->nodeScalarGroupTopDataItem()->attributeBrowserController()->fix(event->pos(), v);
}

void Post2dWindowNodeVectorStreamlineDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->resultDataItem()->nodeScalarGroupTopDataItem()->attributeBrowserController()->update(event->pos(), v);
}

void Post2dWindowNodeVectorStreamlineDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(groupDataItem()->resultDataItem()->nodeScalarGroupTopDataItem()->showAttributeBrowserAction());
}

void Post2dWindowNodeVectorStreamlineDataItem::showPropertyDialog()
{
	groupDataItem()->showPropertyDialog();
}

QDialog* Post2dWindowNodeVectorStreamlineDataItem::propertyDialog(QWidget* parent)
{
	return groupDataItem()->propertyDialog(parent);
}

Post2dWindowNodeVectorStreamlineGroupDataItem* Post2dWindowNodeVectorStreamlineDataItem::groupDataItem() const
{
	return dynamic_cast<Post2dWindowNodeVectorStreamlineGroupDataItem*> (parent());
}
