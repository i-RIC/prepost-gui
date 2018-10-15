#include "graphicswindowrootdataitem_moveupcommand.h"

#include <guibase/objectbrowserview.h>

#include <QStandardItem>

GraphicsWindowRootDataItem::MoveUpCommand::MoveUpCommand(GraphicsWindowDataItem* item, ObjectBrowserView* view) :
	QUndoCommand(QObject::tr("Move up item")),
	m_item {item},
	m_view {view}
{}

void GraphicsWindowRootDataItem::MoveUpCommand::redo()
{
	m_view->setCommandExecution(true);
	m_item->moveUp();
	m_view->select(m_item->standardItem()->index());
	m_view->setCommandExecution(false);
}

void GraphicsWindowRootDataItem::MoveUpCommand::undo()
{
	m_view->setCommandExecution(true);
	m_item->moveDown();
	m_view->select(m_item->standardItem()->index());
	m_view->setCommandExecution(false);
}
