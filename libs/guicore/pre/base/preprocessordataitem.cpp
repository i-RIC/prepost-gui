#include "preprocessordataitem.h"
#include "preprocessorwindowinterface.h"

#include <guibase/objectbrowserview.h>

#include <QStandardItem>

void PreProcessorDataItem::editCaption()
{
	ObjectBrowserView* view = dataModel()->objectBrowserView();
	view->edit(m_standardItem->index());
}

MouseBoundingBox* PreProcessorDataItem::mouseBoundingBox()
{
	return dataModel()->mouseBoundingBox();
}

QAction* PreProcessorDataItem::deleteAction()
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
