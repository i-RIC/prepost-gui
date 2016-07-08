#ifndef INPUTCONDITIONWIDGETREAL_H
#define INPUTCONDITIONWIDGETREAL_H

#include "../inputconditionwidget.h"

#include <QLineEdit>
#include <QMessageBox>
#include <QList>
#include <QEvent>
#include <QtDebug>
#include <cmath>

class RealNumberEditWidget;
class SolverDefinitionTranslator;
class InputConditionContainerReal;

class InputConditionWidgetReal : public InputConditionWidget
{
	Q_OBJECT

public:
	InputConditionWidgetReal(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerReal* cont);

	void addTooltip(const QString& tooltip) override;

	void setDisabled(bool disable);

public slots:
	void setValue(double);
	void getWidgetValue(double);

private:
	InputConditionContainerReal* m_container;
	RealNumberEditWidget* m_lineEdit;
};

#endif // INPUTCONDITIONWIDGETREAL_H
