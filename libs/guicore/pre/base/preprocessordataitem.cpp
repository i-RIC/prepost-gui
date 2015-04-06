#include "preprocessordataitem.h"
#include <guibase/objectbrowserview.h>

#include <QStandardItem>

void PreProcessorDataItem::editCaption()
{
	ObjectBrowserView* view = dataModel()->objectBrowserView();
	view->edit(m_standardItem->index());
}

MouseBoundingBox* PreProcessorDataItem::mouseBoundingBox(){
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
