#ifndef SOLVERCONSOLEWINDOW_IMPL_H
#define SOLVERCONSOLEWINDOW_IMPL_H

#include "../solverconsolewindow.h"

class SolverConsoleWindow::Impl
{
public:
	Impl(iRICMainWindowInterface* mainW, SolverConsoleWindow* w);

	void init();

	void createCancelFile();	  ///< create ".cancel" file to ask solver to stop.
	void removeCancelFile();   	///< remove ".cancel" file.

	QPlainTextEdit* m_console;

	QProcess* m_process;
	bool m_solverKilled;
	bool m_destructing;

	SolverConsoleWindowProjectDataItem* m_projectDataItem;

	ProjectData* m_projectData;
	iRICMainWindowInterface* m_iricMainWindow;

private:
	SolverConsoleWindow* m_window;
};

#endif // SOLVERCONSOLEWINDOW_IMPL_H
