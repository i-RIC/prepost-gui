#ifndef CGNSFILEINPUTCONDITIONWIDGETINTEGER_H
#define CGNSFILEINPUTCONDITIONWIDGETINTEGER_H

#include "../../guicore_global.h"
#include "cgnsfileinputconditionwidget.h"

class QSpinBox;
class CgnsFileInputConditionContainerSet;
class CgnsFileInputConditionContainerInteger;
class SolverDefinitionTranslator;
class QVariant;

class GUICOREDLL_EXPORT CgnsFileInputConditionWidgetInteger : public CgnsFileInputConditionWidget
{
	Q_OBJECT
public:
	CgnsFileInputConditionWidgetInteger(QDomNode defnode, const SolverDefinitionTranslator& t, CgnsFileInputConditionContainerInteger* cont, const CgnsFileInputConditionContainerSet& cs);
	void setMaximum(const QVariant& value) override;
	void setMinimum(const QVariant& value) override;
public slots:
//	void setValue(int) override;
private:
	CgnsFileInputConditionContainerInteger* m_container;
	QSpinBox* m_spinBox;
private slots:
	void informChange(int);
};

#endif // CGNSFILEINPUTCONDITIONWIDGETINTEGER_H
