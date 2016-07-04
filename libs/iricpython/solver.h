#ifndef IRICPYTHON_SOLVER_H
#define IRICPYTHON_SOLVER_H

#include "iricpython_api.h"

class SolverConsoleWindow;

namespace iRICPython {

class Application;

class IRICPYTHON_API Solver
{
public:
	Solver(const Application* app);
	~Solver();

	void start();
	void stop();

	void startAndWaitForFinish();

private:
	SolverConsoleWindow* solverConsole();

	const Application* m_app;
};

} // iRICPython

#endif // IRICPYTHON_SOLVER_H
