#include "graph2dwindowdataitem_standarditemchangecommand.h"

Graph2dWindowDataItem::StandardItemChangeCommand::StandardItemChangeCommand(Graph2dWindowDataItem* item) :
	QUndoCommand(QObject::tr("Object Browser Item Change"))
{
	m_oldDisplayText = item->m_standardItemCopy->data(Qt::DisplayRole);
	m_oldCheckState = item->m_standardItemCopy->data(Qt::CheckStateRole);

	m_newDisplayText = item->m_standardItem->data(Qt::DisplayRole);
	m_newCheckState = item->m_standardItem->data(Qt::CheckStateRole);

	m_item = item;
}

void Graph2dWindowDataItem::StandardItemChangeCommand::redo()
{
	m_item->setIsCommandExecuting(true);

	m_item->m_standardItemCopy->setData(m_newDisplayText, Qt::DisplayRole);
	m_item->m_standardItemCopy->setData(m_newCheckState, Qt::CheckStateRole);

	m_item->m_standardItem->setData(m_newDisplayText, Qt::DisplayRole);
	m_item->m_standardItem->setData(m_newCheckState, Qt::CheckStateRole);

	m_item->setModified();
	m_item->updateVisibility();
	m_item->setIsCommandExecuting(false);
}

void Graph2dWindowDataItem::StandardItemChangeCommand::undo()
{
	m_item->setIsCommandExecuting(true);

	m_item->m_standardItemCopy->setData(m_oldDisplayText, Qt::DisplayRole);
	m_item->m_standardItemCopy->setData(m_oldCheckState, Qt::CheckStateRole);

	m_item->m_standardItem->setData(m_oldDisplayText, Qt::DisplayRole);
	m_item->m_standardItem->setData(m_oldCheckState, Qt::CheckStateRole);

	m_item->setModified();
	m_item->updateVisibility();
	m_item->setIsCommandExecuting(false);
}
