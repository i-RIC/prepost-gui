#ifndef INPUTCONDITIONWIDGETSTRING_H
#define INPUTCONDITIONWIDGETSTRING_H

#include "../../guicore_global.h"
#include "inputconditionwidget.h"

class AsciiOnlyLineEdit;
class InputConditionContainerString;
class SolverDefinitionTranslator;

class GUICOREDLL_EXPORT InputConditionWidgetString : public InputConditionWidget
{
	Q_OBJECT

public:
	InputConditionWidgetString(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont);

public slots:
	void setValue(const QString& newvalue);

private slots:
	void handleEditingFinished();

private:
	InputConditionContainerString* m_container;
	AsciiOnlyLineEdit* m_lineEdit;
};
#endif // INPUTCONDITIONWIDGETSTRING_H
