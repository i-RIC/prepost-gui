#ifndef MODELADDDIALOG_H
#define MODELADDDIALOG_H

#include <QDialog>

namespace Ui {
class ModelAddDialog;
}

class iRICMIProject;
class SolverDefinitionAbstract;
class SolverDefinitionList;

class ModelAddDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ModelAddDialog(iRICMIProject* project, SolverDefinitionList* solvers, QWidget *parent = nullptr);
	~ModelAddDialog();

	std::string folderName() const;
	SolverDefinitionAbstract* solverDefinition() const;

private slots:
	void checkInput();

private:
	void setupSolverList();

	bool checkFolderName();
	bool checkSolver();

	SolverDefinitionList* m_solvers;
	iRICMIProject* m_project;
	Ui::ModelAddDialog *ui;
};

#endif // MODELADDDIALOG_H
