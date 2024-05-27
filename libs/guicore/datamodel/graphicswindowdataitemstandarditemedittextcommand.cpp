#include "graphicswindowdataitem.h"
#include "graphicswindowdataitemstandarditemedittextcommand.h"

GraphicsWindowDataItemStandardItemEditTextCommand::GraphicsWindowDataItemStandardItemEditTextCommand(const QString& text, GraphicsWindowDataItem* item) :
	m_newText {text},
	m_oldText {item->m_standardItem->text()},
	m_item {item}
{}

void GraphicsWindowDataItemStandardItemEditTextCommand::redo()
{
	applyText(m_newText);
}

void GraphicsWindowDataItemStandardItemEditTextCommand::undo()
{
	applyText(m_oldText);
}

void GraphicsWindowDataItemStandardItemEditTextCommand::applyText(const QString& text)
{
	m_item->setIsCommandExecuting(true);

	m_item->m_standardItem->setData(text, Qt::DisplayRole);
	m_item->m_standardItemCopy->setData(text, Qt::DisplayRole);

	m_item->setIsCommandExecuting(false);
}
