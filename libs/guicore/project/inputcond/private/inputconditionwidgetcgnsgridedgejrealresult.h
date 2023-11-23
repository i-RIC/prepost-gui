#ifndef INPUTCONDITIONWIDGETCGNSGRIDEDGEJREALRESULT_H
#define INPUTCONDITIONWIDGETCGNSGRIDEDGEJREALRESULT_H

#include "inputconditionwidgetcgnsresult.h"

class InputConditionWidgetCgnsGridEdgeJRealResult : public InputConditionWidgetCgnsResult
{
public:
	InputConditionWidgetCgnsGridEdgeJRealResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file, bool noStretch);

private:
	QStringList getEnums() const override;
};

#endif // INPUTCONDITIONWIDGETCGNSGRIDEDGEJREALRESULT_H
