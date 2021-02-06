#ifndef INPUTCONDITIONWIDGETCGNSGRIDEDGEIINTEGERRESULT_H
#define INPUTCONDITIONWIDGETCGNSGRIDEDGEIINTEGERRESULT_H

#include "inputconditionwidgetcgnsresult.h"

class InputConditionWidgetCgnsGridEdgeIIntegerResult : public InputConditionWidgetCgnsResult
{
public:
	InputConditionWidgetCgnsGridEdgeIIntegerResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file);

private:
	QStringList getEnums() const override;
};

#endif // INPUTCONDITIONWIDGETCGNSGRIDEDGEIINTEGERRESULT_H
