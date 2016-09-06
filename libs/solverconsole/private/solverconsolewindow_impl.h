#ifndef SOLVERCONSOLEWINDOW_IMPL_H
#define SOLVERCONSOLEWINDOW_IMPL_H

#include "../solverconsolewindow.h"

class SolverConsoleWindow::Impl
{
public:
	Impl(SolverConsoleWindow* w);

private:
	SolverConsoleWindow* m_window;
};

#endif // SOLVERCONSOLEWINDOW_IMPL_H
