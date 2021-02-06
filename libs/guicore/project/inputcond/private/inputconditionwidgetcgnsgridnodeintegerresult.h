#ifndef INPUTCONDITIONWIDGETCGNSGRIDNODEINTEGERRESULT_H
#define INPUTCONDITIONWIDGETCGNSGRIDNODEINTEGERRESULT_H

#include "inputconditionwidgetcgnsresult.h"

class InputConditionWidgetCgnsGridNodeIntegerResult : public InputConditionWidgetCgnsResult
{
public:
	InputConditionWidgetCgnsGridNodeIntegerResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file);

private:
	QStringList getEnums() const override;
};

#endif // INPUTCONDITIONWIDGETCGNSGRIDNODEINTEGERRESULT_H
