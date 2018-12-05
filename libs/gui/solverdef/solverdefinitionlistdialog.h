#ifndef SOLVERDEFINITIONLISTDIALOG_H
#define SOLVERDEFINITIONLISTDIALOG_H

#include <QDialog>

#include <vector>

class SolverDefinitionAbstract;

namespace Ui
{
	class SolverDefinitionListDialog;
}

/// Dialog to show the list of solvers currently installed.
class SolverDefinitionListDialog : public QDialog
{
	Q_OBJECT

public:
	SolverDefinitionListDialog(const std::vector<SolverDefinitionAbstract*>& list, QWidget* parent = nullptr);
	~SolverDefinitionListDialog();

	int execToSelectSolver();
	int selectedSolver() const;

public slots:
	/// Handler for double clicking on solver definition table;
	void handleCellDoubleClick(int row, int column);
	/// Show detail dialog about the solver currently selected
	void showDetailOfCurrent();

private:
	void changeEvent(QEvent* e) override;

	void setup();
	void showDetail(int index);

private:
	Ui::SolverDefinitionListDialog* ui;
	std::vector<SolverDefinitionAbstract*> m_solverList;
};

#endif // SOLVERDEFINITIONLISTDIALOG_H
