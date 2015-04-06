#ifndef CGNSFILEINPUTCONDITIONWIDGETREAL_H
#define CGNSFILEINPUTCONDITIONWIDGETREAL_H

#include "../../guicore_global.h"
#include "cgnsfileinputconditionwidget.h"

#include <QLineEdit>
#include <QMessageBox>
#include <QList>
#include <QEvent>
#include <QtDebug>
#include <cmath>

class RealNumberEditWidget;
class SolverDefinitionTranslator;
class CgnsFileInputConditionContainerReal;

class GUICOREDLL_EXPORT CgnsFileInputConditionWidgetReal : public CgnsFileInputConditionWidget
{
	Q_OBJECT
public:
	CgnsFileInputConditionWidgetReal(QDomNode defnode, const SolverDefinitionTranslator& t, CgnsFileInputConditionContainerReal* cont);
	void setDisabled(bool disable);

public slots:
	void setValue(double);
	void getWidgetValue(double);

private:
	CgnsFileInputConditionContainerReal* m_Container;
	double maxValue;
	bool maxIsSet;
	double minValue;
	bool minIsSet;
	RealNumberEditWidget* m_lineEdit;
};

#endif // CGNSFILEINPUTCONDITIONWIDGETREAL_H
