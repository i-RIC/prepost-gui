#include "gridbirdeyewindownodescalargroupdataitem_updatesettingcommand.h"

#include <misc/qundocommandhelper.h>

GridBirdEyeWindowNodeScalarGroupDataItem::UpdateSettingCommand::UpdateSettingCommand(bool apply, QUndoCommand* settingCommand, QUndoCommand* colorMapCommand) :
	m_apply {apply},
	m_settingCommand {settingCommand},
	m_colorMapCommand {colorMapCommand}
{}

GridBirdEyeWindowNodeScalarGroupDataItem::UpdateSettingCommand::~UpdateSettingCommand()
{
	delete m_settingCommand;
	delete m_colorMapCommand;
}

void GridBirdEyeWindowNodeScalarGroupDataItem::UpdateSettingCommand::redo()
{
	m_settingCommand->redo();
	if (m_colorMapCommand != nullptr) {
		m_colorMapCommand->redo();
	}
}

void GridBirdEyeWindowNodeScalarGroupDataItem::UpdateSettingCommand::undo()
{
	m_settingCommand->undo();
	if (m_colorMapCommand != nullptr) {
		m_colorMapCommand->undo();
	}
}

int GridBirdEyeWindowNodeScalarGroupDataItem::UpdateSettingCommand::id() const
{
	return iRIC::generateCommandId("GridBirdEyeWindowNodeScalarGroupDataItem::UpdateSetting");
}

bool GridBirdEyeWindowNodeScalarGroupDataItem::UpdateSettingCommand::mergeWith(const QUndoCommand* other)
{
	if (! m_apply) {return false;}

	auto other2 = const_cast<QUndoCommand*> (other);
	auto c = dynamic_cast<UpdateSettingCommand*> (other2);
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