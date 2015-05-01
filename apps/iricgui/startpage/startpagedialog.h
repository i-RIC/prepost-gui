#ifndef STARTPAGEDIALOG_H
#define STARTPAGEDIALOG_H

#include <QDialog>

namespace Ui
{
	class StartPageDialog;
}

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
	explicit StartPageDialog(QWidget* parent = nullptr);
	~StartPageDialog();
	CommandMode commandMode() {return m_commandMode;}
	SolverDefinitionAbstract* solverDefinition() {return m_solverDefinition;}
	const QString& projectFileName() {return m_projectFileName;}
	void setSolverList(SolverDefinitionList* solverList);
	void setLocale(const QString& locale);
private slots:
	void handleNewProjectButtonClick();
	void handleOpenProjectButtonClick();
	void handleProjectFileSelection(const QString& filename);
	void handleSolverSelection(SolverDefinitionAbstract* solverDef);
private:
	Ui::StartPageDialog* ui;
	CommandMode m_commandMode;
	SolverDefinitionAbstract* m_solverDefinition;
	QString m_projectFileName;
};

#endif // STARTPAGEDIALOG_H
