#include "post3dwindowparticlesbasescalargroupdataitem_updateactorsettingcommand.h"

#include <misc/qundocommandhelper.h>

Post3dWindowParticlesBaseScalarGroupDataItem::UpdateActorSettingCommand::UpdateActorSettingCommand(bool apply, QUndoCommand* settingCommand, QUndoCommand* colorMapCommand, Post3dWindowParticlesBaseScalarGroupDataItem* item) :
	m_apply {apply},
	m_settingCommand {settingCommand},
	m_colorMapCommand {colorMapCommand},
	m_item {item}
{}

void Post3dWindowParticlesBaseScalarGroupDataItem::UpdateActorSettingCommand::redo()
{
	m_settingCommand->redo();
	if (m_colorMapCommand != nullptr) {
		m_colorMapCommand->redo();
	}
	m_item->updateCheckState();
	m_item->updateActorSetting();
}

void Post3dWindowParticlesBaseScalarGroupDataItem::UpdateActorSettingCommand::undo()
{
	m_settingCommand->undo();
	if (m_colorMapCommand != nullptr) {
		m_colorMapCommand->undo();
	}
	m_item->updateCheckState();
	m_item->updateActorSetting();
}

int Post3dWindowParticlesBaseScalarGroupDataItem::UpdateActorSettingCommand::id() const
{
	return iRIC::generateCommandId("Post3dWindowParticlesBaseScalarGroupDataItem::UpdateActorSettingsCommand");
}

bool Post3dWindowParticlesBaseScalarGroupDataItem::UpdateActorSettingCommand::mergeWith(const QUndoCommand* other)
{
	if (! m_apply) {return false;}

	auto other2 = const_cast<QUndoCommand*> (other);
	auto c = dynamic_cast<UpdateActorSettingCommand*> (other2);
	if (c == nullptr) {return false;}

	m_settingCommand->mergeWith(c->m_settingCommand);

	if (m_colorMapCommand == nullptr) {
		m_colorMapCommand = c->m_colorMapCommand;
		c->m_colorMapCommand = nullptr;
	} else if (c->m_colorMapCommand == nullptr) {
		m_colorMapCommand->undo();
		delete m_colorMapCommand;
		m_colorMapCommand = nullptr;
	} else {
		bool ok = m_colorMapCommand->mergeWith(c->m_colorMapCommand);

		if (!ok) {
			m_colorMapCommand->undo();
			delete m_colorMapCommand;
			m_colorMapCommand = c->m_colorMapCommand;
			c->m_colorMapCommand = nullptr;
		}
	}
	m_apply = c->m_apply;
	return true;
}
