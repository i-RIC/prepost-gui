#include "graphicswindowrootdataitem_movedowncommand.h"

#include <guibase/objectbrowserview.h>

#include <QStandardItem>

GraphicsWindowRootDataItem::MoveDownCommand::MoveDownCommand(GraphicsWindowDataItem* item, ObjectBrowserView* view) :
	QUndoCommand(QObject::tr("Move down item")),
	m_item {item},
	m_view {view}
{}

void GraphicsWindowRootDataItem::MoveDownCommand::redo()
{
	m_view->setCommandExecution(true);
	m_item->moveDown();
	m_view->select(m_item->standardItem()->index());
	m_view->setCommandExecution(false);
}

void GraphicsWindowRootDataItem::MoveDownCommand::undo()
{
	m_view->setCommandExecution(true);
	m_item->moveUp();
	m_view->select(m_item->standardItem()->index());
	m_view->setCommandExecution(false);
}
