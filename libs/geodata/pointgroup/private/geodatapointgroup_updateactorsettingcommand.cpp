#include "geodatapointgroup_updateactorsettingcommand.h"

GeoDataPointGroup::UpdateActorSettingCommand::UpdateActorSettingCommand(QUndoCommand* command, GeoDataPointGroup* group) :
	QUndoCommand {},
	m_command {command},
	m_group {group}
{}

void GeoDataPointGroup::UpdateActorSettingCommand::redo()
{
	m_command->redo();
	m_group->updateActorSetting();
}

void GeoDataPointGroup::UpdateActorSettingCommand::undo()
{
	m_command->undo();
	m_group->updateActorSetting();
}

int GeoDataPointGroup::UpdateActorSettingCommand::id() const
{
	return m_command->id();
}

bool GeoDataPointGroup::UpdateActorSettingCommand::mergeWith(const QUndoCommand* other)
{
	auto c = dynamic_cast<const UpdateActorSettingCommand*> (other);
	if (c == nullptr) {return false;}

	return m_command->mergeWith(c->m_command.get());
}
