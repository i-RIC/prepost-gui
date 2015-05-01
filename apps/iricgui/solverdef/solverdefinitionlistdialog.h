#ifndef SOLVERDEFINITIONLISTDIALOG_H
#define SOLVERDEFINITIONLISTDIALOG_H

#include <QDialog>

class SolverDefinitionList;

namespace Ui
{
	class SolverDefinitionListDialog;
}

/// Dialog to show the list of solvers currently installed.
class SolverDefinitionListDialog : public QDialog
{
	Q_OBJECT
public:
	/// Constructor
	SolverDefinitionListDialog(SolverDefinitionList* list, QWidget* parent = nullptr);
	/// Destructor
	~SolverDefinitionListDialog();
	/// Setup dialog items
	void setup();
	/// Show detail dialog on the specified index (row)
	void showDetail(int index);
public slots:
	/// Handler for double clicking on solver definition table;
	void handleCellDoubleClick(int row, int column);
	/// Show detail dialog about the solver currently selected
	void showDetailOfCurrent();
protected:
	void changeEvent(QEvent* e);
private:
	Ui::SolverDefinitionListDialog* ui;
	SolverDefinitionList* m_solverList;
};

#endif // SOLVERDEFINITIONLISTDIALOG_H
