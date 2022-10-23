#include "post2dwindowparticlesbasescalargroupdataitem_updateactorsettingscommand.h"

#include <misc/qundocommandhelper.h>

Post2dWindowParticlesBaseScalarGroupDataItem::UpdateActorSettingsCommand::UpdateActorSettingsCommand(bool apply, QUndoCommand* settingCommand, QUndoCommand* colorMapCommand, Post2dWindowParticlesBaseScalarGroupDataItem* item) :
	m_apply {apply},
	m_settingCommand {settingCommand},
	m_colorMapCommand {colorMapCommand},
	m_item {item}
{}

void Post2dWindowParticlesBaseScalarGroupDataItem::UpdateActorSettingsCommand::redo()
{
	m_settingCommand->redo();
	if (m_colorMapCommand != nullptr) {
		m_colorMapCommand->redo();
	}
	m_item->updateCheckState();
	m_item->updateActorSettings();
}

void Post2dWindowParticlesBaseScalarGroupDataItem::UpdateActorSettingsCommand::undo()
{
	m_settingCommand->undo();
	if (m_colorMapCommand != nullptr) {
		m_colorMapCommand->undo();
	}
	m_item->updateCheckState();
	m_item->updateActorSettings();
}

int Post2dWindowParticlesBaseScalarGroupDataItem::UpdateActorSettingsCommand::id() const
{
	return iRIC::generateCommandId("Post2dWindowParticlesBaseScalarGroupDataItem::UpdateActorSettingsCommand");
}

bool Post2dWindowParticlesBaseScalarGroupDataItem::UpdateActorSettingsCommand::mergeWith(const QUndoCommand* other)
{
	if (! m_apply) {return false;}

	auto other2 = const_cast<QUndoCommand*> (other);
	auto c = dynamic_cast<UpdateActorSettingsCommand*> (other2);
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
