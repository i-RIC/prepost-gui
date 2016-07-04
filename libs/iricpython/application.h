#ifndef IRICPYTHON_APPLICATION_H
#define IRICPYTHON_APPLICATION_H

#include "iricpython_api.h"
#include <string>

class PreProcessorWindow;
class ProjectData;
class SolverConsoleWindow;

namespace iRICPython {

class Pre;
class Solver;
class CalculationResult;

class IRICPYTHON_API Application
{
public:
	static void init(const char* path);

	Application();
	~Application();

	bool openProject(const std::string& filename);
	bool saveProject();
	bool saveProjectAs(const std::string& filename);
	bool closeProject();

	Pre* pre() const;
	Solver* solver() const;
	CalculationResult* calcResult() const;

private:
	PreProcessorWindow* preWindow() const;
	SolverConsoleWindow* solverConsole() const;
	ProjectData* projectData() const;

	class Impl;
	Impl* impl;

public:
	friend class Pre;
	friend class Solver;
	friend class CalculationResult;
};

} // iRICPython

#ifdef _DEBUG
	#include "private/application_impl.h"
#endif // _DEBUG

#endif // IRICPYTHON_APPLICATION_H
