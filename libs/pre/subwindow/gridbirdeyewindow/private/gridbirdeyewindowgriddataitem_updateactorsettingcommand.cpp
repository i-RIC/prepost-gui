#include "gridbirdeyewindowgriddataitem_updateactorsettingcommand.h"

GridBirdEyeWindowGridDataItem::UpdateActorSettingCommand::UpdateActorSettingCommand(QUndoCommand* command, GridBirdEyeWindowGridDataItem* item) :
	QUndoCommand {},
	m_command {command},
	m_item {item}
{}

void GridBirdEyeWindowGridDataItem::UpdateActorSettingCommand::redo()
{
	m_command->redo();
	m_item->updateActorSettings();
}

void GridBirdEyeWindowGridDataItem::UpdateActorSettingCommand::undo()
{
	m_command->undo();
	m_item->updateActorSettings();
}

int GridBirdEyeWindowGridDataItem::UpdateActorSettingCommand::id() const
{
	return m_command->id();
}

bool GridBirdEyeWindowGridDataItem::UpdateActorSettingCommand::mergeWith(const QUndoCommand* other)
{
	auto c = dynamic_cast<const UpdateActorSettingCommand*> (other);
	if (c == nullptr) {return false;}

	return m_command->mergeWith(c->m_command.get());
}
