#ifndef INPUTCONDITIONWIDGETREALOPTION_H
#define INPUTCONDITIONWIDGETREALOPTION_H

#include "../../guicore_global.h"
#include "inputconditionwidget.h"

#include <QComboBox>
class InputConditionContainerReal;
class SolverDefinitionTranslator;

class GUICOREDLL_EXPORT InputConditionWidgetRealOption : public InputConditionWidget
{
	Q_OBJECT

public:
	InputConditionWidgetRealOption(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerReal* cont);
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
