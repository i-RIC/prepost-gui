#include "preprocessordataitem.h"
#include "preprocessorwindowinterface.h"

#include <guibase/objectbrowserview.h>

#include <QStandardItem>


PreProcessorDataItem::PreProcessorDataItem(GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem {parent}
{}

PreProcessorDataItem::PreProcessorDataItem(const QString& itemlabel, GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem {itemlabel, parent}
{}

PreProcessorDataItem::PreProcessorDataItem(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem {itemlabel, icon, parent}
{}

PreProcessorDataItem::PreProcessorDataItem(ProjectDataItem* parent) :
	GraphicsWindowDataItem {parent}
{}

PreProcessorDataItem::~PreProcessorDataItem()
{}

void PreProcessorDataItem::editCaption()
{
	ObjectBrowserView* view = dataModel()->objectBrowserView();
	view->edit(m_standardItem->index());
}

MouseBoundingBox* PreProcessorDataItem::mouseBoundingBox()
{
	return dataModel()->mouseBoundingBox();
}

QAction* PreProcessorDataItem::deleteAction() const
{
	ObjectBrowserView* view = dataModel()->objectBrowserView();
	return view->deleteAction();
}

void PreProcessorDataItem::copyStandardItem()
{
	m_standardItemCopy = m_standardItem->clone();
}

PreProcessorWindowInterface* PreProcessorDataItem::preProcessorWindow() const
{
	return dynamic_cast<PreProcessorWindowInterface*>(mainWindow());
}

PreProcessorDataModelInterface* PreProcessorDataItem::dataModel() const
{
	return dynamic_cast<PreProcessorDataModelInterface*>(GraphicsWindowDataItem::dataModel());
}
