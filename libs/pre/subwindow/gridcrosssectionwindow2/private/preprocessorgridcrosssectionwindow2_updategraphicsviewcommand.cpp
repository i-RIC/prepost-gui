#include "preprocessorgridcrosssectionwindow2_updategraphicsviewcommand.h"

#include <misc/qundocommandhelper.h>

PreProcessorGridCrosssectionWindow2::UpdateGraphicsViewCommand::UpdateGraphicsViewCommand(QUndoCommand* command, PreProcessorGridCrosssectionWindow2* window) :
	m_command {command},
	m_window {window}
{}

void PreProcessorGridCrosssectionWindow2::UpdateGraphicsViewCommand::redo()
{
	m_command->redo();
	m_window->updateGraphicsView();
}

void PreProcessorGridCrosssectionWindow2::UpdateGraphicsViewCommand::undo()
{
	m_command->undo();
	m_window->updateGraphicsView();
}

int PreProcessorGridCrosssectionWindow2::UpdateGraphicsViewCommand::id() const
{
	return iRIC::generateCommandId("PreProcessorGridCrosssectionWindow2::UpdateGraphicsViewCommand");
}

bool PreProcessorGridCrosssectionWindow2::UpdateGraphicsViewCommand::mergeWith(const QUndoCommand* other)
{
	auto c = dynamic_cast<const UpdateGraphicsViewCommand*> (other);
	if (c == nullptr) {return false;}

	return m_command->mergeWith(c->m_command.get());
}
