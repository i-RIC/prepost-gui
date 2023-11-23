#ifndef INPUTCONDITIONWIDGETCGNSGRIDNODEREALRESULT_H
#define INPUTCONDITIONWIDGETCGNSGRIDNODEREALRESULT_H

#include "inputconditionwidgetcgnsresult.h"

class InputConditionWidgetCgnsGridNodeRealResult : public InputConditionWidgetCgnsResult
{
public:
	InputConditionWidgetCgnsGridNodeRealResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file, bool noStretch);

private:
	QStringList getEnums() const override;
};

#endif // INPUTCONDITIONWIDGETCGNSGRIDNODEREALRESULT_H
