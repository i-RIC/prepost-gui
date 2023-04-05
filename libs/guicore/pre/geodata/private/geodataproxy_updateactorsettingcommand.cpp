#include "geodataproxy_updateactorsettingcommand.h"

GeoDataProxy::UpdateActorSettingCommand::UpdateActorSettingCommand(QUndoCommand* command, GeoDataProxy* proxy) :
	QUndoCommand {},
	m_command {command},
	m_proxy {proxy}
{}

void GeoDataProxy::UpdateActorSettingCommand::redo()
{
	m_command->redo();
	m_proxy->updateActorSetting();
}

void GeoDataProxy::UpdateActorSettingCommand::undo()
{
	m_command->undo();
	m_proxy->updateActorSetting();
}

int GeoDataProxy::UpdateActorSettingCommand::id() const
{
	return m_command->id();
}

bool GeoDataProxy::UpdateActorSettingCommand::mergeWith(const QUndoCommand* other)
{
	auto c = dynamic_cast<const UpdateActorSettingCommand*> (other);
	if (c == nullptr) {return false;}

	return m_command->mergeWith(c->m_command.get());
}
