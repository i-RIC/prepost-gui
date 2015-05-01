#ifndef CGNSFILEINPUTCONDITIONWIDGETINTEGEROPTION_H
#define CGNSFILEINPUTCONDITIONWIDGETINTEGEROPTION_H

#include "../../guicore_global.h"
#include "cgnsfileinputconditionwidget.h"

#include <QComboBox>
class CgnsFileInputConditionContainerInteger;
class SolverDefinitionTranslator;

class GUICOREDLL_EXPORT CgnsFileInputConditionWidgetIntegerOption : public CgnsFileInputConditionWidget
{
	Q_OBJECT
public:
	CgnsFileInputConditionWidgetIntegerOption(QDomNode defnode, const SolverDefinitionTranslator& t, CgnsFileInputConditionContainerInteger* cont);
	void setDisabled(bool disable) {
		m_comboBox->setDisabled(disable);
	}

public slots:
	void setValue(int);

private:
	CgnsFileInputConditionContainerInteger* m_container;
	QComboBox* m_comboBox;

private slots:
	void informChange(int);
};

#endif // CGNSFILEINPUTCONDITIONWIDGETINTEGEROPTION_H
