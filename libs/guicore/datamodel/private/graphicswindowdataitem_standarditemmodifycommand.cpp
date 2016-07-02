#include "graphicswindowdataitem_standarditemmodifycommand.h"

GraphicsWindowDataItem::StandardItemModifyCommand::StandardItemModifyCommand(QUndoCommand *child, GraphicsWindowDataItem* item) :
	QUndoCommand {child->text()},
	m_command {child},
	m_item {item}
{}

GraphicsWindowDataItem::StandardItemModifyCommand::~StandardItemModifyCommand()
{}

void GraphicsWindowDataItem::StandardItemModifyCommand::redo()
{
	m_item->setIsCommandExecuting(true);
	m_command.get()->redo();
	m_item->setIsCommandExecuting(false);
}

void GraphicsWindowDataItem::StandardItemModifyCommand::undo()
{
	m_item->setIsCommandExecuting(true);
	m_command.get()->undo();
	m_item->setIsCommandExecuting(false);
}

int GraphicsWindowDataItem::StandardItemModifyCommand::id() const
{
	return m_command->id();
}

bool GraphicsWindowDataItem::StandardItemModifyCommand::mergeWith(const QUndoCommand *other)
{
	const StandardItemModifyCommand* modc = dynamic_cast<const StandardItemModifyCommand*> (other);
	if (modc == nullptr) {return false;}

	return m_command.get()->mergeWith(modc->m_command.get());
}
