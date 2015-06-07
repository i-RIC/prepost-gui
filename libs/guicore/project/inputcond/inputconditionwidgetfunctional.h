#ifndef CGNSFILEINPUTCONDITIONWIDGETFUNCTIONAL_H
#define CGNSFILEINPUTCONDITIONWIDGETFUNCTIONAL_H

#include "../../guicore_global.h"
#include "inputconditionwidget.h"

class InputConditionContainerFunctional;
class SolverDefinitionTranslator;
class InputConditionContainerSet;
class InputConditionWidgetFunctionalDialog;
class iRICMainWindow;

class GUICOREDLL_EXPORT InputConditionWidgetFunctional : public InputConditionWidget
{
	Q_OBJECT

public:
	InputConditionWidgetFunctional(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerFunctional* cont);
	~InputConditionWidgetFunctional();
	void setDisabled(bool /*disable*/) {}

private:
	InputConditionContainerFunctional* m_container;
	const SolverDefinitionTranslator* m_translator;
	InputConditionWidgetFunctionalDialog* m_dialog;

private slots:
	void openDialog();
	void dialogAccepted();

public:
	friend class InputConditionWidgetFunctionalDialog;
};

#endif
