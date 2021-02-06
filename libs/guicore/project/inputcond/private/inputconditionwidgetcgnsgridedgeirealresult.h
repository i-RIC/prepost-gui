#ifndef INPUTCONDITIONWIDGETCGNSGRIDEDGEIREALRESULT_H
#define INPUTCONDITIONWIDGETCGNSGRIDEDGEIREALRESULT_H

#include "inputconditionwidgetcgnsresult.h"

class InputConditionWidgetCgnsGridEdgeIRealResult : public InputConditionWidgetCgnsResult
{
public:
	InputConditionWidgetCgnsGridEdgeIRealResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file);

private:
	QStringList getEnums() const override;
};

#endif // INPUTCONDITIONWIDGETCGNSGRIDEDGEIREALRESULT_H
