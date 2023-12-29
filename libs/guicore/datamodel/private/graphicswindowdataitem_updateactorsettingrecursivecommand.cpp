#include "graphicswindowdataitem_updateactorsettingrecursivecommand.h"

GraphicsWindowDataItem::UpdateActorSettingRecursiveCommand::UpdateActorSettingRecursiveCommand(QUndoCommand *child, GraphicsWindowDataItem* item) :
	QUndoCommand {child->text()},
	m_command {child},
	m_item {item}
{}

void GraphicsWindowDataItem::UpdateActorSettingRecursiveCommand::redo()
{
	m_command.get()->redo();
	m_item->updateActorSettingRecursively();
}

void GraphicsWindowDataItem::UpdateActorSettingRecursiveCommand::undo()
{
	m_command.get()->undo();
	m_item->updateActorSettingRecursively();
}

int GraphicsWindowDataItem::UpdateActorSettingRecursiveCommand::id() const
{
	return m_command->id();
}

bool GraphicsWindowDataItem::UpdateActorSettingRecursiveCommand::mergeWith(const QUndoCommand *other)
{
	const auto modc = dynamic_cast<const UpdateActorSettingRecursiveCommand*> (other);
	if (modc == nullptr) {return false;}

	return m_command.get()->mergeWith(modc->m_command.get());
}
