#include "graphicswindowdataitem_updateactorsettingcommand.h"

GraphicsWindowDataItem::UpdateActorSettingCommand::UpdateActorSettingCommand(QUndoCommand *child, GraphicsWindowDataItem* item) :
	QUndoCommand {child->text()},
	m_command {child},
	m_item {item}
{}

void GraphicsWindowDataItem::UpdateActorSettingCommand::redo()
{
	m_command.get()->redo();
	m_item->updateActorSetting();
}

void GraphicsWindowDataItem::UpdateActorSettingCommand::undo()
{
	m_command.get()->undo();
	m_item->updateActorSetting();
}

int GraphicsWindowDataItem::UpdateActorSettingCommand::id() const
{
	return m_command->id();
}

bool GraphicsWindowDataItem::UpdateActorSettingCommand::mergeWith(const QUndoCommand *other)
{
	const UpdateActorSettingCommand* modc = dynamic_cast<const UpdateActorSettingCommand*> (other);
	if (modc == nullptr) {return false;}

	return m_command.get()->mergeWith(modc->m_command.get());
}
