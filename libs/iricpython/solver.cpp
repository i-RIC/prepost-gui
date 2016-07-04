#include "application.h"
#include "solver.h"

#include <solverconsole/solverconsolewindow.h>

using namespace iRICPython;

Solver::Solver(const Application* app) :
	m_app {app}
{}

Solver::~Solver()
{}

void Solver::start()
{
	solverConsole()->startSolverSilently();
}

void Solver::stop()
{
	solverConsole()->terminateSolverSilently();
}

void Solver::startAndWaitForFinish()
{
	auto console = solverConsole();

	console->startSolverSilently();
	console->waitForSolverFinish();
}

SolverConsoleWindow* Solver::solverConsole()
{
	return m_app->solverConsole();
}
