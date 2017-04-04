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
	SolverDefinitionAbstractWidget(const SolverDefinitionAbstract* solver, QWidget* parent = nullptr);
	~SolverDefinitionAbstractWidget();

private:
	void setup();

	Ui::SolverDefinitionAbstractWidget* ui;
	const SolverDefinitionAbstract* m_solverDefinition;
};

#endif // SOLVERDEFINITIONABSTRACTWIDGET_H
