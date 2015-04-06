#ifndef CGNSFILEINPUTCONDITIONWIDGETSTRING_H
#define CGNSFILEINPUTCONDITIONWIDGETSTRING_H

#include "../../guicore_global.h"
#include "cgnsfileinputconditionwidget.h"

class QLineEdit;
class CgnsFileInputConditionContainerString;
class SolverDefinitionTranslator;

class GUICOREDLL_EXPORT CgnsFileInputConditionWidgetString : public CgnsFileInputConditionWidget
{
	Q_OBJECT
public:
	CgnsFileInputConditionWidgetString(QDomNode defnode, const SolverDefinitionTranslator& t, CgnsFileInputConditionContainerString* cont);
	void setDisabled(bool disable);

public slots:
	void setValue(const QString& newvalue);

private slots:
	void getWidgetValue(const QString& newvalue);
	void checkContent();

private:
	CgnsFileInputConditionContainerString* m_container;
	QLineEdit* m_lineEdit;
};
#endif // CGNSFILEINPUTCONDITIONWIDGETSTRING_H
