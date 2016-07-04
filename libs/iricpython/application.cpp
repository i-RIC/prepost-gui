#include "application.h"
#include "calculationresult.h"
#include "pre.h"
#include "solver.h"
#include "private/application_impl.h"

#include <guicore/project/projectdata.h>
#include <misc/iricrootpath.h>
#include <pre/preprocessorwindow.h>

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>

#include <vtkAutoInit.h>

using namespace iRICPython;

namespace {

QApplication* staticQApp = nullptr;

void myMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
	// ignore all messages
}

}

VTK_MODULE_INIT(vtkRenderingOpenGL);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingFreeType);
VTK_MODULE_INIT(vtkRenderingFreeTypeOpenGL);

Application::Impl::Impl() :
	m_mainWindow {true},
	m_pre {nullptr},
	m_solver {nullptr},
	m_calcResult {nullptr}
{}

Application::Impl::~Impl()
{
	delete m_pre;
	delete m_solver;
	delete m_calcResult;
}

// public interfaces

void Application::init(const char *path)
{
	QCoreApplication::setOrganizationName("iRIC Organization");
	QCoreApplication::setOrganizationDomain("i-ric.org");
	QCoreApplication::setApplicationName("iRIC GUI");

	iRICRootPath::set(path);

	int argc = 1;
	char appName[] = "iRICPython";
	char* argv = &(appName[0]);

	qInstallMessageHandler(myMessageOutput);

	staticQApp = new QApplication(argc, &argv);
}

Application::Application() :
	impl {new Impl()}
{}

Application::~Application()
{
	impl->m_mainWindow.closeProject();
	delete impl;
}

bool Application::openProject(const std::string& filename)
{
	impl->m_mainWindow.openProject(filename.c_str());
	return true;
}

bool Application::saveProject()
{
	return impl->m_mainWindow.saveProject();
}

bool Application::saveProjectAs(const std::string& filename)
{
	return impl->m_mainWindow.saveProject(filename.c_str(), false);
}

bool Application::closeProject()
{
	return impl->m_mainWindow.closeProject();
}

Pre* Application::pre() const
{
	if (impl->m_pre == nullptr) {
		impl->m_pre = new Pre(this);
	}
	return impl->m_pre;
}

Solver* Application::solver() const
{
	if (impl->m_solver == nullptr) {
		impl->m_solver = new Solver(this);
	}
	return impl->m_solver;
}

CalculationResult* Application::calcResult() const
{
	if (impl->m_calcResult == nullptr) {
		impl->m_calcResult = new CalculationResult(this);
	}
	return impl->m_calcResult;
}

PreProcessorWindow* Application::preWindow() const
{
	return dynamic_cast<PreProcessorWindow*> (impl->m_mainWindow.preProcessorWindow());
}

SolverConsoleWindow* Application::solverConsole() const
{
	return impl->m_mainWindow.solverConsoleWindow();
}

ProjectData* Application::projectData() const
{
	return impl->m_mainWindow.m_projectData;
}
