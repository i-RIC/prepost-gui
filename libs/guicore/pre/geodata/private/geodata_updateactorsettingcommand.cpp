#include "geodata_updateactorsettingcommand.h"

GeoData::UpdateActorSettingCommand::UpdateActorSettingCommand(QUndoCommand* command, GeoData* geoData) :
	QUndoCommand {},
	m_command {command},
	m_geoData {geoData}
{}

void GeoData::UpdateActorSettingCommand::redo()
{
	m_command->redo();
	m_geoData->updateActorSetting();
}

void GeoData::UpdateActorSettingCommand::undo()
{
	m_command->undo();
	m_geoData->updateActorSetting();
}

int GeoData::UpdateActorSettingCommand::id() const
{
	return m_command->id();
}

bool GeoData::UpdateActorSettingCommand::mergeWith(const QUndoCommand* other)
{
	auto c = dynamic_cast<const UpdateActorSettingCommand*> (other);
	if (c == nullptr) {return false;}

	return m_command->mergeWith(c->m_command.get());
}
