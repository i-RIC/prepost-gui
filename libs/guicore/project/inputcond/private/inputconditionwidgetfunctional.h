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
	void setDisabled(bool disable);

private slots:
	void openDialog();
	void dialogAccepted();

private:
	InputConditionContainerFunctional* m_container;
	const SolverDefinitionTranslator* m_translator;
	InputConditionWidgetFunctionalDialog* m_dialog;

public:
	friend class InputConditionWidgetFunctionalDialog;
};

#endif // INPUTCONDITIONWIDGETFUNCTIONAL_H
