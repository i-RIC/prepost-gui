#ifndef SOLVERCONSOLEWINDOW_IMPL_H
#define SOLVERCONSOLEWINDOW_IMPL_H

#include "../solverconsolewindow.h"

class QPlainTextEdit;

class SolverConsoleWindow::Impl
{
public:
	Impl(iRICMainWindowI* mainW, SolverConsoleWindow* w);

	void init();

	void createCancelFile();	  ///< create ".cancel" file to ask solver to stop.
	void removeCancelFile();   	///< remove ".cancel" file.
	void removeCancelOkFile();  ///< remove ".cancel_ok" file.

	void appendLogLine(const QString& line);

	QPlainTextEdit* m_console;

	QProcess* m_process;
	bool m_solverKilled;
	bool m_destructing;

	SolverConsoleWindowProjectDataItem* m_projectDataItem;

	ProjectData* m_projectData;
	iRICMainWindowI* m_iricMainWindow;

private:
	SolverConsoleWindow* m_window;
};

#endif // SOLVERCONSOLEWINDOW_IMPL_H
