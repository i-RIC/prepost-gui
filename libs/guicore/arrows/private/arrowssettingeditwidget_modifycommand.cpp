#include "arrowssettingeditwidget_modifycommand.h"

#include <misc/qundocommandhelper.h>

ArrowsSettingEditWidget::ModifyCommand::ModifyCommand(QUndoCommand* as, QUndoCommand* cs) :
	m_arrowsSettingCommand {as},
	m_colormapSettingCommand {cs}
{}

ArrowsSettingEditWidget::ModifyCommand::~ModifyCommand()
{
	delete m_arrowsSettingCommand;
	delete m_colormapSettingCommand;
}

void ArrowsSettingEditWidget::ModifyCommand::redo()
{
	m_arrowsSettingCommand->redo();
	if (m_colormapSettingCommand != nullptr) {
		m_colormapSettingCommand->redo();
	}
}

void ArrowsSettingEditWidget::ModifyCommand::undo()
{
	m_arrowsSettingCommand->undo();
	if (m_colormapSettingCommand != nullptr) {
		m_colormapSettingCommand->undo();
	}
}

int ArrowsSettingEditWidget::ModifyCommand::id() const
{
	return iRIC::generateCommandId("ArrowsSettingEditWidget::ModifyCommand");
}

bool ArrowsSettingEditWidget::ModifyCommand::mergeWith(const QUndoCommand *other)
{
	auto other2 = const_cast<QUndoCommand*> (other);
	auto c = dynamic_cast<ModifyCommand*> (other2);
	if (c == nullptr) {return false;}

	m_arrowsSettingCommand->mergeWith(c->m_arrowsSettingCommand);

	if (m_colormapSettingCommand == nullptr) {
		m_colormapSettingCommand = c->m_colormapSettingCommand;
		c->m_colormapSettingCommand = nullptr;
	} else if (c->m_colormapSettingCommand == nullptr) {
		m_colormapSettingCommand->undo();
		delete m_colormapSettingCommand;
		m_colormapSettingCommand = nullptr;
	} else {
		bool ok = m_colormapSettingCommand->mergeWith(c->m_colormapSettingCommand);

		if (!ok) {
			m_colormapSettingCommand->undo();
			delete m_colormapSettingCommand;
			m_colormapSettingCommand = c->m_colormapSettingCommand;
			c->m_colormapSettingCommand = nullptr;
		}
	}
	return true;
}
