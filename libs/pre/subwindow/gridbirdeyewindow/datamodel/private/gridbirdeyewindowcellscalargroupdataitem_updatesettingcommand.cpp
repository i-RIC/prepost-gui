#include "gridbirdeyewindowcellscalargroupdataitem_updatesettingcommand.h"

#include <misc/qundocommandhelper.h>

GridBirdEyeWindowCellScalarGroupDataItem::UpdateSettingCommand::UpdateSettingCommand(bool apply, QUndoCommand* settingCommand, QUndoCommand* colorMapCommand) :
	m_apply {apply},
	m_settingCommand {settingCommand},
	m_colorMapCommand {colorMapCommand}
{}

GridBirdEyeWindowCellScalarGroupDataItem::UpdateSettingCommand::~UpdateSettingCommand()
{
	delete m_settingCommand;
	delete m_colorMapCommand;
}

void GridBirdEyeWindowCellScalarGroupDataItem::UpdateSettingCommand::redo()
{
	m_settingCommand->redo();
	if (m_colorMapCommand != nullptr) {
		m_colorMapCommand->redo();
	}
}

void GridBirdEyeWindowCellScalarGroupDataItem::UpdateSettingCommand::undo()
{
	m_settingCommand->undo();
	if (m_colorMapCommand != nullptr) {
		m_colorMapCommand->undo();
	}
}

int GridBirdEyeWindowCellScalarGroupDataItem::UpdateSettingCommand::id() const
{
	return iRIC::generateCommandId("GridBirdEyeWindowCellScalarGroupDataItem::UpdateSetting");
}

bool GridBirdEyeWindowCellScalarGroupDataItem::UpdateSettingCommand::mergeWith(const QUndoCommand* other)
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
