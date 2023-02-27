#include "post2dwindownodevectorarrowdataitem.h"
#include "post2dwindownodevectorarrowgroupdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/datamodel/vtkgraphicsview.h>

#include <QMenu>
#include <QMouseEvent>
#include <QStandardItem>

Post2dWindowNodeVectorArrowDataItem::Post2dWindowNodeVectorArrowDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{}

Post2dWindowNodeVectorArrowDataItem::~Post2dWindowNodeVectorArrowDataItem()
{}

void Post2dWindowNodeVectorArrowDataItem::informSelection(VTKGraphicsView* v)
{
	groupDataItem()->informSelection(v);
}

void Post2dWindowNodeVectorArrowDataItem::informDeselection(VTKGraphicsView* v)
{
	groupDataItem()->informDeselection(v);
}

void Post2dWindowNodeVectorArrowDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mouseMoveEvent(event, v);
}

void Post2dWindowNodeVectorArrowDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mousePressEvent(event, v);
}

void Post2dWindowNodeVectorArrowDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mouseReleaseEvent(event, v);
}

void Post2dWindowNodeVectorArrowDataItem::addCustomMenuItems(QMenu* menu)
{
	QAction* abAction = dynamic_cast<Post2dWindowZoneDataItem*>(parent()->parent())->showAttributeBrowserActionForNodeResult();
	menu->addAction(abAction);
}

Post2dWindowNodeVectorArrowGroupDataItem* Post2dWindowNodeVectorArrowDataItem::groupDataItem() const
{
	return dynamic_cast<Post2dWindowNodeVectorArrowGroupDataItem*> (parent());
}
