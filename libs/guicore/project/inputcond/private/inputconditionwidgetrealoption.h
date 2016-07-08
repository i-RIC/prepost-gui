#ifndef INPUTCONDITIONWIDGETREALOPTION_H
#define INPUTCONDITIONWIDGETREALOPTION_H

#include "../inputconditionwidget.h"

class InputConditionContainerReal;
class SolverDefinitionTranslator;

class QComboBox;

class InputConditionWidgetRealOption : public InputConditionWidget
{
	Q_OBJECT

public:
	InputConditionWidgetRealOption(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerReal* cont);

	void addTooltip(const QString& tooltip) override;

	void setDisabled(bool disable);

public slots:
	void setValue(double);

private slots:
	void informChange(int);

private:
	InputConditionContainerReal* m_container;
	QComboBox* m_comboBox;
};

#endif // INPUTCONDITIONWIDGETREALOPTION_H
