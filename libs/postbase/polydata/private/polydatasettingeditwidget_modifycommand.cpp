#include "polydatasettingeditwidget_modifycommand.h"

#include <misc/qundocommandhelper.h>

PolyDataSettingEditWidget::ModifyCommand::ModifyCommand(bool allowMerge, QUndoCommand* settingCommand, QUndoCommand* colorMapCommand) :
	QUndoCommand (),
	m_allowMerge {allowMerge},
	m_settingCommand {settingCommand},
	m_colorMapCommand {colorMapCommand}
{}


PolyDataSettingEditWidget::ModifyCommand::~ModifyCommand()
{
	delete m_settingCommand;
	delete m_colorMapCommand;
}

void PolyDataSettingEditWidget::ModifyCommand::redo()
{
	m_settingCommand->redo();
	if (m_colorMapCommand != nullptr) {
		m_colorMapCommand->redo();
	}
}

void PolyDataSettingEditWidget::ModifyCommand::undo()
{
	m_settingCommand->undo();
	if (m_colorMapCommand != nullptr) {
		m_colorMapCommand->undo();
	}
}

int PolyDataSettingEditWidget::ModifyCommand::id() const
{
	return iRIC::generateCommandId("PolyDataSettingEditWidget::ModifyCommand");
}

bool PolyDataSettingEditWidget::ModifyCommand::mergeWith(const QUndoCommand* other)
{
	if (! m_allowMerge) {return false;}

	auto other2 = const_cast<QUndoCommand*> (other);
	auto c = dynamic_cast<ModifyCommand*> (other2);
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
	m_allowMerge = c->m_allowMerge;
	return true;
}
