#ifndef STARTPAGEDIALOG_H
#define STARTPAGEDIALOG_H

#include <QDialog>

namespace Ui
{
	class StartPageDialog;
}

class iRICMainWindow;
class SolverDefinitionList;
class SolverDefinitionAbstract;

class StartPageDialog : public QDialog
{
	Q_OBJECT

public:
	enum CommandMode {
		cmNewProject,
		cmNewOtherProject,
		cmOpenProject,
		cmOpenOtherProject,
		cmNone,
	};
	explicit StartPageDialog(iRICMainWindow* parent = nullptr);
	~StartPageDialog();

	CommandMode commandMode() const;
	SolverDefinitionAbstract* solverDefinition() const;
	const QString& projectFileName() const;

	void setSolverList(SolverDefinitionList* solverList);
	void setLocale(const QString& locale);

private slots:
	void handleNewProjectButtonClick();
	void handleOpenProjectButtonClick();
	void handleProjectFileSelection(const QString& filename);
	void handleSolverSelection(SolverDefinitionAbstract* solverDef);
	void handleLaunchMiGui();

private:
	Ui::StartPageDialog* ui;

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/startpagedialog_impl.h"
#endif // _DEBUG

#endif // STARTPAGEDIALOG_H
