#ifndef CGNSFILEINPUTCONDITIONWIDGETREALOPTION_H
#define CGNSFILEINPUTCONDITIONWIDGETREALOPTION_H

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
	void setDisabled(bool disable) {
		m_comboBox->setDisabled(disable);
	}

public slots:
	void setValue(double);

private:
	InputConditionContainerReal* m_container;
	QComboBox* m_comboBox;

private slots:
	void informChange(int);
};

#endif // CGNSFILEINPUTCONDITIONWIDGETREALOPTION_H
