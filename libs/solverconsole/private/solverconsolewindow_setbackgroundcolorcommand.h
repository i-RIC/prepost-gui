#ifndef SOLVERCONSOLEWINDOW_SETBACKGROUNDCOLORCOMMAND_H
#define SOLVERCONSOLEWINDOW_SETBACKGROUNDCOLORCOMMAND_H

#include "../solverconsolewindow.h"

#include <QUndoCommand>

class SolverConsoleWindow::SetBackgroundColorCommand : public QUndoCommand
{
public:
	SetBackgroundColorCommand(const QColor& newColor, SolverConsoleWindow* w);

	void redo() override;
	void undo() override;

private:
	QColor m_newColor;
	QColor m_oldColor;

	SolverConsoleWindow* m_window;
};

#endif // SOLVERCONSOLEWINDOW_SETBACKGROUNDCOLORCOMMAND_H
