#ifndef INPUTCONDITIONWIDGETINTEGEROPTION_H
#define INPUTCONDITIONWIDGETINTEGEROPTION_H

#include "../../guicore_global.h"
#include "inputconditionwidget.h"

#include <QComboBox>
class InputConditionContainerInteger;
class SolverDefinitionTranslator;

class GUICOREDLL_EXPORT InputConditionWidgetIntegerOption : public InputConditionWidget
{
	Q_OBJECT

public:
	InputConditionWidgetIntegerOption(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerInteger* cont);
	void setDisabled(bool disable);

public slots:
	void setValue(int);

private slots:
	void informChange(int);

private:
	InputConditionContainerInteger* m_container;
	QComboBox* m_comboBox;
};

#endif // INPUTCONDITIONWIDGETINTEGEROPTION_H
