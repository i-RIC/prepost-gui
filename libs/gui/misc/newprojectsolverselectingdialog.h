#ifndef NEWPROJECTSOLVERSELECTINGDIALOG_H
#define NEWPROJECTSOLVERSELECTINGDIALOG_H

#include <QDialog>

namespace Ui
{
	class NewProjectSolverSelectingDialog;
}

class SolverDefinitionList;
class SolverDefinitionAbstract;

class NewProjectSolverSelectingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit NewProjectSolverSelectingDialog(SolverDefinitionList* list, QWidget* parent = nullptr);
	~NewProjectSolverSelectingDialog();
	SolverDefinitionAbstract* selectedSolver();
	void setSolver(const QString& folderName);

private slots:
	void listDoubleClicked();

private:
	SolverDefinitionList* m_solverList;
	Ui::NewProjectSolverSelectingDialog* ui;
};

#endif // NEWPROJECTSOLVERSELECTINGDIALOG_H
