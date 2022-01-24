#ifndef INPUTCONDITIONWIDGETSTRING_H
#define INPUTCONDITIONWIDGETSTRING_H

#include "../inputconditionwidget.h"

class TextEditI;
class InputConditionContainerString;
class SolverDefinitionTranslator;

#include "../inputconditiondependency.h"  // can't forward declare nested classes (InputConditionDependency::ActionSetValue)

class InputConditionWidgetString : public InputConditionWidget
{
	Q_OBJECT

public:
	InputConditionWidgetString(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont);

	void addTooltip(const QString& tooltip) override;

public slots:
	void setValue(const QString& newvalue);

private slots:
	void handleEditingFinished();

private:
	InputConditionContainerString* m_container;
	TextEditI* m_textEdit;

public:
	friend class InputConditionDependency::ActionSetValue;
};
#endif // INPUTCONDITIONWIDGETSTRING_H
