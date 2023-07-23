#ifndef IRICMIMAINWINDOW_IMPL_H
#define IRICMIMAINWINDOW_IMPL_H

#include "../iricmimainwindow.h"

#include <QString>
#include <QLocale>

class iRICMIProject;
class SolverDefinitionList;

class iRICMIMainWindow::Impl
{
public:
	Impl(iRICMIMainWindow* mainWindow);
	~Impl();

	void initViews();
	void resetViews(bool enable);
	void makeConnections();

	bool newProject();
	bool openProject();
	bool saveProject();
	bool closeProject();

	void initLocale();
	void setupSolverDefinitionList();
	void updateModelsColumnWidths();
	void updateWindowTitle();

	iRICMIProject* m_project;
	SolverDefinitionList* m_solverDefinitionList;
	QLocale m_locale;

private:
	iRICMIMainWindow* m_mainWindow;
};

#endif // IRICMIMAINWINDOW_IMPL_H
