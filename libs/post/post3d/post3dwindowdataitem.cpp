#include "post3dwindowdataitem.h"

Post3dWindowDataItem::Post3dWindowDataItem(GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem(parent)
{}

Post3dWindowDataItem::Post3dWindowDataItem(const QString& itemlabel, GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem(itemlabel, parent)
{}

Post3dWindowDataItem::Post3dWindowDataItem(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem(itemlabel, icon, parent)
{}

Post3dWindowDataItem::~Post3dWindowDataItem()
{}

Post3dWindow* Post3dWindowDataItem::postProcessorWindow() const
{
	return dynamic_cast<Post3dWindow*>(dataModel()->mainWindow());
}

void Post3dWindowDataItem::updateZDepthRangeItemCount()
{}

void Post3dWindowDataItem::assignActorZValues(const ZDepthRange& )
{}

Post3dWindowDataModel* Post3dWindowDataItem::dataModel() const
{
	return dynamic_cast<Post3dWindowDataModel*>(GraphicsWindowDataItem::dataModel());
}
