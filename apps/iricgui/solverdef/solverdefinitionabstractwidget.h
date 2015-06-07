#ifndef SOLVERDEFINITIONABSTRACTWIDGET_H
#define SOLVERDEFINITIONABSTRACTWIDGET_H

class SolverDefinitionAbstract;

#include <QWidget>

namespace Ui
{
	class SolverDefinitionAbstractWidget;
}

/// This class defines the dialog to show information about a solver,
/// like name, version, release date etc.
class SolverDefinitionAbstractWidget : public QWidget
{
	Q_OBJECT

public:
	/// Constructor
	SolverDefinitionAbstractWidget(const SolverDefinitionAbstract* solver, QWidget* parent = nullptr);
	/// Destructor
	~SolverDefinitionAbstractWidget();

private:
	/// Setup dialog widgets to show the information about the solver
	void setup();
	Ui::SolverDefinitionAbstractWidget* ui;
	/// The container that stores solver information
	const SolverDefinitionAbstract* m_solverDefinition;
};

#endif // SOLVERDEFINITIONABSTRACTWIDGET_H
