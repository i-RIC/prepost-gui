#ifndef INPUTCONDITIONWIDGETCHECKBOX_H
#define INPUTCONDITIONWIDGETCHECKBOX_H

#include "../inputconditionwidget.h"

class InputConditionContainerInteger;
class SolverDefinitionTranslator;

class QCheckBox;

class InputConditionWidgetCheckbox : public InputConditionWidget
{
public:
	Q_OBJECT

public:
	InputConditionWidgetCheckbox(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerInteger* cont);

	void addTooltip(const QString& tooltip) override;

	void setDisabled(bool disable);

public slots:
	void setValue(int);

private slots:
	void informChange(bool checked);

private:
	InputConditionContainerInteger* m_container;
	QCheckBox* m_checkBox;
};

#endif // INPUTCONDITIONWIDGETCHECKBOX_H
