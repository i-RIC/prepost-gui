#include "abstractcrosssectionwindow_updategraphicsviewcommand.h"

#include <misc/qundocommandhelper.h>

AbstractCrosssectionWindow::UpdateGraphicsViewCommand::UpdateGraphicsViewCommand(QUndoCommand* command, AbstractCrosssectionWindow* window) :
	m_command {command},
	m_window {window}
{}

void AbstractCrosssectionWindow::UpdateGraphicsViewCommand::redo()
{
	m_command->redo();
	m_window->updateGraphicsView();
}

void AbstractCrosssectionWindow::UpdateGraphicsViewCommand::undo()
{
	m_command->undo();
	m_window->updateGraphicsView();
}

int AbstractCrosssectionWindow::UpdateGraphicsViewCommand::id() const
{
	return iRIC::generateCommandId("AbstractCrosssectionWindow::UpdateGraphicsViewCommand");
}

bool AbstractCrosssectionWindow::UpdateGraphicsViewCommand::mergeWith(const QUndoCommand* other)
{
	auto c = dynamic_cast<const UpdateGraphicsViewCommand*> (other);
	if (c == nullptr) {return false;}

	return m_command->mergeWith(c->m_command.get());
}
