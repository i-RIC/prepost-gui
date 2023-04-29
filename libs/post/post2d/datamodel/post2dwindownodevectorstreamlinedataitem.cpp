#include "post2dwindownodevectorstreamlinedataitem.h"
#include "post2dwindownodevectorstreamlinegroupdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/postcontainer/postzonedatacontainer.h>

#include <QAction>
#include <QMenu>
#include <QMouseEvent>

#include <vtkStructuredGrid.h>

Post2dWindowNodeVectorStreamlineDataItem::Post2dWindowNodeVectorStreamlineDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{}

Post2dWindowNodeVectorStreamlineDataItem::~Post2dWindowNodeVectorStreamlineDataItem()
{}

void Post2dWindowNodeVectorStreamlineDataItem::informSelection(VTKGraphicsView*)
{
	zoneDataItem()->initNodeResultAttributeBrowser();
}

void Post2dWindowNodeVectorStreamlineDataItem::informDeselection(VTKGraphicsView*)
{
	zoneDataItem()->clearNodeResultAttributeBrowser();
}

void Post2dWindowNodeVectorStreamlineDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	zoneDataItem()->updateNodeResultAttributeBrowser(event->pos(), v);
}

void Post2dWindowNodeVectorStreamlineDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	zoneDataItem()->fixNodeResultAttributeBrowser(event->pos(), v);
}

void Post2dWindowNodeVectorStreamlineDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* abAction = dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->showAttributeBrowserActionForNodeResult();
	menu->addAction(abAction);
}

void Post2dWindowNodeVectorStreamlineDataItem::showPropertyDialog()
{
	groupDataItem()->showPropertyDialog();
}

QDialog* Post2dWindowNodeVectorStreamlineDataItem::propertyDialog(QWidget* parent)
{
	return groupDataItem()->propertyDialog(parent);
}

Post2dWindowZoneDataItem* Post2dWindowNodeVectorStreamlineDataItem::zoneDataItem() const
{
	return dynamic_cast<Post2dWindowZoneDataItem*> (parent()->parent());
}

Post2dWindowNodeVectorStreamlineGroupDataItem* Post2dWindowNodeVectorStreamlineDataItem::groupDataItem() const
{
	return dynamic_cast<Post2dWindowNodeVectorStreamlineGroupDataItem*> (parent());
}
