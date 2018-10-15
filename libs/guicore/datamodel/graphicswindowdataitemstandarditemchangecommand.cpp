#include "graphicswindowdataitem.h"
#include "graphicswindowdataitemstandarditemchangecommand.h"

#include <QStandardItem>

namespace {

void applySettingToQStandardItem(const QVariant& text, const QVariant& checkState, QStandardItem* item)
{
	item->setData(text, Qt::DisplayRole);
	item->setData(checkState, Qt::CheckStateRole);
}

} // namespace

GraphicsWindowDataItemStandardItemChangeCommand::GraphicsWindowDataItemStandardItemChangeCommand(GraphicsWindowDataItem* item) :
	QUndoCommand(GraphicsWindowDataItem::tr("Object Browser Item Change")),
	m_newDisplayText (item->m_standardItem->data(Qt::DisplayRole)),
	m_newCheckState (item->m_standardItem->data(Qt::CheckStateRole)),
	m_oldDisplayText (item->m_standardItemCopy->data(Qt::DisplayRole)),
	m_oldCheckState (item->m_standardItemCopy->data(Qt::CheckStateRole)),
	m_item {item}
{}

GraphicsWindowDataItemStandardItemChangeCommand::~GraphicsWindowDataItemStandardItemChangeCommand()
{}

void GraphicsWindowDataItemStandardItemChangeCommand::redo()
{
	m_item->setIsCommandExecuting(true);

	redoStandardItem();
	m_item->updateVisibility();

	m_item->setIsCommandExecuting(false);
}

void GraphicsWindowDataItemStandardItemChangeCommand::redoStandardItem() const
{
	applySetting(m_newDisplayText, m_newCheckState);
}

void GraphicsWindowDataItemStandardItemChangeCommand::undo()
{
	m_item->setIsCommandExecuting(true);

	undoStandardItem();
	m_item->updateVisibility();

	m_item->setIsCommandExecuting(false);
}

void GraphicsWindowDataItemStandardItemChangeCommand::undoStandardItem() const
{
	applySetting(m_oldDisplayText, m_oldCheckState);
}

void GraphicsWindowDataItemStandardItemChangeCommand::applySetting(const QVariant& text, const QVariant& checkState) const
{
	applySettingToQStandardItem(text, checkState, m_item->m_standardItem);
	applySettingToQStandardItem(text, checkState, m_item->m_standardItemCopy);

	m_item->setModified();
}
