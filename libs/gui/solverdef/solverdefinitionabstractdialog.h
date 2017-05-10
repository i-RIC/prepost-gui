#ifndef SOLVERDEFINITIONABSTRACTDIALOG_H
#define SOLVERDEFINITIONABSTRACTDIALOG_H

class SolverDefinitionAbstract;

#include <QDialog>

namespace Ui
{
	class SolverDefinitionAbstractDialog;
}

/// This class defines the dialog to show information about a solver,
/// like name, version, release date etc.
class SolverDefinitionAbstractDialog : public QDialog
{
	Q_OBJECT

public:
	SolverDefinitionAbstractDialog(const SolverDefinitionAbstract* solver, QWidget* parent = nullptr);
	~SolverDefinitionAbstractDialog();

private:
	void changeEvent(QEvent* e) override;

private:
	void setup();

	Ui::SolverDefinitionAbstractDialog* ui;
	const SolverDefinitionAbstract* m_solverDefinition;
};

#endif // SOLVERDEFINITIONABSTRACTDIALOG_H
