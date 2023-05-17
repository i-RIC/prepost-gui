#include "gridbirdeyewindow.h"
#include "gridbirdeyewindowdataitem.h"
#include "gridbirdeyewindowdatamodel.h"

GridBirdEyeWindowDataItem::GridBirdEyeWindowDataItem(GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem(parent)
{}

GridBirdEyeWindowDataItem::GridBirdEyeWindowDataItem(const QString& itemlabel, GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem(itemlabel, parent)
{}

GridBirdEyeWindowDataItem::GridBirdEyeWindowDataItem(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem(itemlabel, icon, parent)
{}

GridBirdEyeWindow* GridBirdEyeWindowDataItem::mainWindow()
{
	return dynamic_cast<GridBirdEyeWindow*>(dataModel()->mainWindow());
}

GridBirdEyeWindowDataModel* GridBirdEyeWindowDataItem::dataModel() const
{
	return dynamic_cast<GridBirdEyeWindowDataModel*>(GraphicsWindowDataItem::dataModel());
}
