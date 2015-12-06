#include "post2dwindow.h"
#include "post2dwindowdataitem.h"
#include "post2dwindowdatamodel.h"

Post2dWindowDataItem::Post2dWindowDataItem(GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem(parent)
{}

Post2dWindowDataItem::Post2dWindowDataItem(const QString& itemlabel, GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem(itemlabel, parent)
{}

Post2dWindowDataItem::Post2dWindowDataItem(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem(itemlabel, icon, parent)
{}

Post2dWindowDataItem::~Post2dWindowDataItem()
{}

Post2dWindow* Post2dWindowDataItem::postProcessorWindow() const
{
	return dynamic_cast<Post2dWindow*>(dataModel()->mainWindow());
}

Post2dWindowDataModel* Post2dWindowDataItem::dataModel() const
{
	return dynamic_cast<Post2dWindowDataModel*>(GraphicsWindowDataItem::dataModel());
}
