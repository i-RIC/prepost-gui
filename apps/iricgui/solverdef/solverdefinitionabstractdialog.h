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
	/// Constructor
	SolverDefinitionAbstractDialog(const SolverDefinitionAbstract* solver, QWidget* parent = nullptr);
	/// Destructor
	~SolverDefinitionAbstractDialog();

protected:
	void changeEvent(QEvent* e) override;

private:
	/// Setup dialog widgets to show the information about the solver
	void setup();
	Ui::SolverDefinitionAbstractDialog* ui;
	/// The container that stores solver information
	const SolverDefinitionAbstract* m_solverDefinition;
};

#endif // SOLVERDEFINITIONABSTRACTDIALOG_H
