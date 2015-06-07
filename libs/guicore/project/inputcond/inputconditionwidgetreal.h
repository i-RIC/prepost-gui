#ifndef CGNSFILEINPUTCONDITIONWIDGETREAL_H
#define CGNSFILEINPUTCONDITIONWIDGETREAL_H

#include "../../guicore_global.h"
#include "inputconditionwidget.h"

#include <QLineEdit>
#include <QMessageBox>
#include <QList>
#include <QEvent>
#include <QtDebug>
#include <cmath>

class RealNumberEditWidget;
class SolverDefinitionTranslator;
class InputConditionContainerReal;

class GUICOREDLL_EXPORT InputConditionWidgetReal : public InputConditionWidget
{
	Q_OBJECT
public:
	InputConditionWidgetReal(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerReal* cont);
	void setDisabled(bool disable);

public slots:
	void setValue(double);
	void getWidgetValue(double);

private:
	InputConditionContainerReal* m_Container;
	double maxValue;
	bool maxIsSet;
	double minValue;
	bool minIsSet;
	RealNumberEditWidget* m_lineEdit;
};

#endif // CGNSFILEINPUTCONDITIONWIDGETREAL_H
