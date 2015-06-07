#ifndef CGNSFILEINPUTCONDITIONWIDGETINTEGEROPTION_H
#define CGNSFILEINPUTCONDITIONWIDGETINTEGEROPTION_H

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
	void setDisabled(bool disable) {
		m_comboBox->setDisabled(disable);
	}

public slots:
	void setValue(int);

private:
	InputConditionContainerInteger* m_container;
	QComboBox* m_comboBox;

private slots:
	void informChange(int);
};

#endif // CGNSFILEINPUTCONDITIONWIDGETINTEGEROPTION_H
