#ifndef IRICPYTHON_APPLICATION_IMPL_H
#define IRICPYTHON_APPLICATION_IMPL_H

#include "../application.h"

#include <gui/main/iricmainwindow.h>

#include <QLocale>

class ProjectData;

namespace iRICPython {

class Application::Impl
{
public:
	Impl();
	~Impl();

	iRICMainWindow m_mainWindow;
	Pre* m_pre;
	Solver* m_solver;
	CalculationResult* m_calcResult;
};

} // iRICPython

#endif // IRICPYTHON_APPLICATION_IMPL_H
