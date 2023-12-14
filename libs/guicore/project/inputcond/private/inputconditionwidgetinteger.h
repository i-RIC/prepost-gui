#ifndef INPUTCONDITIONWIDGETINTEGER_H
#define INPUTCONDITIONWIDGETINTEGER_H

#include "../inputconditionwidget.h"

class InputConditionContainerSet;
class InputConditionContainerInteger;
class SolverDefinitionTranslator;

class QSpinBox;
class QVariant;

#include "../inputconditiondependency.h"  // can't forward declare nested classes (InputConditionDependency::ActionSetValue)

class InputConditionWidgetInteger : public InputConditionWidget
{
	Q_OBJECT

public:
	InputConditionWidgetInteger(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerInteger* cont, const InputConditionContainerSet& cs, bool noStretch);

	void addTooltip(const QString& tooltip) override;

private slots:
	void informChange(int);

private:
	InputConditionContainerInteger* m_container;
	QSpinBox* m_spinBox;

public:
	friend class InputConditionDependency::ActionSetValue;
};

#endif // INPUTCONDITIONWIDGETINTEGER_H
