#ifndef INPUTCONDITIONWIDGETINTEGER_H
#define INPUTCONDITIONWIDGETINTEGER_H

#include "../inputconditionwidget.h"

class InputConditionContainerSet;
class InputConditionContainerInteger;
class SolverDefinitionTranslator;

class QSpinBox;
class QVariant;

class InputConditionWidgetInteger : public InputConditionWidget
{
	Q_OBJECT

public:
	InputConditionWidgetInteger(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerInteger* cont, const InputConditionContainerSet& cs);

	void addTooltip(const QString& tooltip) override;

	void setMaximum(const QVariant& value) override;
	void setMinimum(const QVariant& value) override;

private slots:
	void informChange(int);

private:
	InputConditionContainerInteger* m_container;
	QSpinBox* m_spinBox;

};

#endif // INPUTCONDITIONWIDGETINTEGER_H
