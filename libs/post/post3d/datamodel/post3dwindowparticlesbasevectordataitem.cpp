#include "post3dwindowparticlesbasevectordataitem.h"
#include "post3dwindowparticlesbasevectorgroupdataitem.h"
#include "post3dwindowparticlesbasetopdataitem.h"

#include <QMenu>

Post3dWindowParticlesBaseVectorDataItem::Post3dWindowParticlesBaseVectorDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent) :
	NamedGraphicWindowDataItem(name, caption, parent)
{}

Post3dWindowParticlesBaseVectorDataItem::~Post3dWindowParticlesBaseVectorDataItem()
{}

void Post3dWindowParticlesBaseVectorDataItem::informSelection(VTKGraphicsView* v)
{
	groupDataItem()->informSelection(v);
}

void Post3dWindowParticlesBaseVectorDataItem::informDeselection(VTKGraphicsView* v)
{
	groupDataItem()->informDeselection(v);
}

void Post3dWindowParticlesBaseVectorDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mouseMoveEvent(event, v);
}

void Post3dWindowParticlesBaseVectorDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mousePressEvent(event, v);
}

void Post3dWindowParticlesBaseVectorDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	groupDataItem()->mouseReleaseEvent(event, v);
}

Post3dWindowParticlesBaseVectorGroupDataItem* Post3dWindowParticlesBaseVectorDataItem::groupDataItem() const
{
	return dynamic_cast<Post3dWindowParticlesBaseVectorGroupDataItem*> (parent());
}
