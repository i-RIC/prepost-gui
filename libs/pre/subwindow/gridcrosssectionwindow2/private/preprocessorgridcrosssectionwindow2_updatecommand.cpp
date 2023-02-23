#include "preprocessorgridcrosssectionwindow2_updatecommand.h"

#include <misc/qundocommandhelper.h>

PreProcessorGridCrosssectionWindow2::UpdateCommand::UpdateCommand(QUndoCommand* command, PreProcessorGridCrosssectionWindow2* window) :
	m_command {command},
	m_window {window}
{}

void PreProcessorGridCrosssectionWindow2::UpdateCommand::redo()
{
	m_command->redo();
	m_window->update();
}

void PreProcessorGridCrosssectionWindow2::UpdateCommand::undo()
{
	m_command->undo();
	m_window->update();
}

int PreProcessorGridCrosssectionWindow2::UpdateCommand::id() const
{
	return iRIC::generateCommandId("PreProcessorGridCrosssectionWindow2::UpdateCommand");
}

bool PreProcessorGridCrosssectionWindow2::UpdateCommand::mergeWith(const QUndoCommand* other)
{
	auto c = dynamic_cast<const UpdateCommand*> (other);
	if (c == nullptr) {return false;}

	return m_command->mergeWith(c->m_command.get());
}
