#include "abstractcrosssectionwindow_updatecommand.h"

#include <misc/qundocommandhelper.h>

AbstractCrosssectionWindow::UpdateCommand::UpdateCommand(QUndoCommand* command, AbstractCrosssectionWindow* window) :
	m_command {command},
	m_window {window}
{}

void AbstractCrosssectionWindow::UpdateCommand::redo()
{
	m_command->redo();
	m_window->update();
}

void AbstractCrosssectionWindow::UpdateCommand::undo()
{
	m_command->undo();
	m_window->update();
}

int AbstractCrosssectionWindow::UpdateCommand::id() const
{
	return iRIC::generateCommandId("AbstractCrosssectionWindow::UpdateCommand");
}

bool AbstractCrosssectionWindow::UpdateCommand::mergeWith(const QUndoCommand* other)
{
	auto c = dynamic_cast<const UpdateCommand*> (other);
	if (c == nullptr) {return false;}

	return m_command->mergeWith(c->m_command.get());
}
