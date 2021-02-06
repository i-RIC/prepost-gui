#ifndef INPUTCONDITIONWIDGETCGNSGRIDEDGEJINTEGERRESULT_H
#define INPUTCONDITIONWIDGETCGNSGRIDEDGEJINTEGERRESULT_H

#include "inputconditionwidgetcgnsresult.h"

class InputConditionWidgetCgnsGridEdgeJIntegerResult : public InputConditionWidgetCgnsResult
{
public:
	InputConditionWidgetCgnsGridEdgeJIntegerResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file);

private:
	QStringList getEnums() const override;
};

#endif // INPUTCONDITIONWIDGETCGNSGRIDEDGEJINTEGERRESULT_H
