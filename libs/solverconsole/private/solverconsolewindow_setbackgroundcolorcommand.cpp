#include "solverconsolewindow_setbackgroundcolorcommand.h"

SolverConsoleWindow::SetBackgroundColorCommand::SetBackgroundColorCommand(const QColor& newColor, SolverConsoleWindow* w) :
	QUndoCommand(SolverConsoleWindow::tr("Background Color Setting")),
	m_newColor {newColor},
	m_oldColor {w->backgroundColor()},
	m_window {w}
{}

void SolverConsoleWindow::SetBackgroundColorCommand::redo()
{
	m_window->setBackgroundColor(m_newColor);
}

void SolverConsoleWindow::SetBackgroundColorCommand::undo()
{
	m_window->setBackgroundColor(m_oldColor);
}
