#ifndef CGNSFILEINPUTCONDITIONWIDGETINTEGER_H
#define CGNSFILEINPUTCONDITIONWIDGETINTEGER_H

#include "../../guicore_global.h"
#include "inputconditionwidget.h"

class QSpinBox;
class InputConditionContainerSet;
class InputConditionContainerInteger;
class SolverDefinitionTranslator;
class QVariant;

class GUICOREDLL_EXPORT InputConditionWidgetInteger : public InputConditionWidget
{
	Q_OBJECT
public:
	InputConditionWidgetInteger(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerInteger* cont, const InputConditionContainerSet& cs);
	void setMaximum(const QVariant& value) override;
	void setMinimum(const QVariant& value) override;
public slots:
//	void setValue(int) override;
private:
	InputConditionContainerInteger* m_container;
	QSpinBox* m_spinBox;
private slots:
	void informChange(int);
};

#endif // CGNSFILEINPUTCONDITIONWIDGETINTEGER_H
