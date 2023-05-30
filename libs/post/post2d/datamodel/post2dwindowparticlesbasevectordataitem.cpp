#include "post2dwindowparticlesbasevectordataitem.h"
#include "post2dwindowparticlesbasevectorgroupdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guicore/datamodel/vtkgraphicsview.h>

#include <QMenu>
#include <QMouseEvent>

Post2dWindowParticlesBaseVectorDataItem::Post2dWindowParticlesBaseVectorDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{}

Post2dWindowParticlesBaseVectorDataItem::~Post2dWindowParticlesBaseVectorDataItem()
{}

void Post2dWindowParticlesBaseVectorDataItem::informSelection(VTKGraphicsView* v)
{
	groupDataItem()->informSelection(v);
}

void Post2dWindowParticlesBaseVectorDataItem::informDeselection(VTKGraphicsView* v)
{
	groupDataItem()->informDeselection(v);
}

void Post2dWindowParticlesBaseVectorDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mouseMoveEvent(event, v);
}

void Post2dWindowParticlesBaseVectorDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mousePressEvent(event, v);
}

void Post2dWindowParticlesBaseVectorDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mouseReleaseEvent(event, v);
}

void Post2dWindowParticlesBaseVectorDataItem::addCustomMenuItems(QMenu* menu)
{
	return groupDataItem()->addCustomMenuItems(menu);
}

bool Post2dWindowParticlesBaseVectorDataItem::addToolBarButtons(QToolBar* toolBar)
{
	return groupDataItem()->addToolBarButtons(toolBar);
}

void Post2dWindowParticlesBaseVectorDataItem::showPropertyDialog()
{
	groupDataItem()->showPropertyDialog();
}

QDialog* Post2dWindowParticlesBaseVectorDataItem::propertyDialog(QWidget* p)
{
	return groupDataItem()->propertyDialog(p);
}

Post2dWindowParticlesBaseVectorGroupDataItem* Post2dWindowParticlesBaseVectorDataItem::groupDataItem() const
{
	return dynamic_cast<Post2dWindowParticlesBaseVectorGroupDataItem*> (parent());
}
