#include "post2dbirdeyewindow.h"
#include "post2dbirdeyewindowdataitem.h"
#include "post2dbirdeyewindowdatamodel.h"

Post2dBirdEyeWindowDataItem::Post2dBirdEyeWindowDataItem(GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem(parent)
{}

Post2dBirdEyeWindowDataItem::Post2dBirdEyeWindowDataItem(const QString& itemlabel, GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem(itemlabel, parent)
{}

Post2dBirdEyeWindowDataItem::Post2dBirdEyeWindowDataItem(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem(itemlabel, icon, parent)
{}

Post2dBirdEyeWindow* Post2dBirdEyeWindowDataItem::postProcessorWindow()
{
	return dynamic_cast<Post2dBirdEyeWindow*>(dataModel()->mainWindow());
}

Post2dBirdEyeWindowDataModel* Post2dBirdEyeWindowDataItem::dataModel()
{
	return dynamic_cast<Post2dBirdEyeWindowDataModel*>(GraphicsWindowDataItem::dataModel());
}
