#ifndef CGNSFILEINPUTCONDITIONWIDGETSTRING_H
#define CGNSFILEINPUTCONDITIONWIDGETSTRING_H

#include "../../guicore_global.h"
#include "inputconditionwidget.h"

class QLineEdit;
class InputConditionContainerString;
class SolverDefinitionTranslator;

class GUICOREDLL_EXPORT InputConditionWidgetString : public InputConditionWidget
{
	Q_OBJECT
public:
	InputConditionWidgetString(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont);
	void setDisabled(bool disable);

public slots:
	void setValue(const QString& newvalue);

private slots:
	void getWidgetValue(const QString& newvalue);
	void checkContent();

private:
	InputConditionContainerString* m_container;
	QLineEdit* m_lineEdit;
};
#endif // CGNSFILEINPUTCONDITIONWIDGETSTRING_H
