#ifndef INPUTCONDITIONWIDGETFUNCTIONAL_H
#define INPUTCONDITIONWIDGETFUNCTIONAL_H

#include "../inputconditionwidget.h"

class InputConditionContainerFunctional;
class SolverDefinitionTranslator;
class InputConditionContainerSet;
class InputConditionWidgetFunctionalDialog;
class iRICMainWindow;

class InputConditionWidgetFunctional : public InputConditionWidget
{
	Q_OBJECT

public:
	InputConditionWidgetFunctional(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerFunctional* cont);
	~InputConditionWidgetFunctional();

	void addTooltip(const QString& tooltip) override;

private slots:
	void openDialog();
	void dialogAccepted();

private:
	InputConditionContainerFunctional* m_container;
	InputConditionWidgetFunctionalDialog* m_dialog;

public:
	friend class InputConditionWidgetFunctionalDialog;
};

#endif // INPUTCONDITIONWIDGETFUNCTIONAL_H
