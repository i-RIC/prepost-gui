#ifndef CGNSFILEINPUTCONDITIONWIDGETREALOPTION_H
#define CGNSFILEINPUTCONDITIONWIDGETREALOPTION_H

#include "../../guicore_global.h"
#include "cgnsfileinputconditionwidget.h"

#include <QComboBox>
class CgnsFileInputConditionContainerReal;
class SolverDefinitionTranslator;

class GUICOREDLL_EXPORT CgnsFileInputConditionWidgetRealOption : public CgnsFileInputConditionWidget
{
	Q_OBJECT

public:
	CgnsFileInputConditionWidgetRealOption(QDomNode defnode, const SolverDefinitionTranslator& t, CgnsFileInputConditionContainerReal* cont);
	void setDisabled(bool disable) {
		m_comboBox->setDisabled(disable);
	}

public slots:
	void setValue(double);

private:
	CgnsFileInputConditionContainerReal* m_container;
	QComboBox* m_comboBox;

private slots:
	void informChange(int);
};

#endif // CGNSFILEINPUTCONDITIONWIDGETREALOPTION_H
