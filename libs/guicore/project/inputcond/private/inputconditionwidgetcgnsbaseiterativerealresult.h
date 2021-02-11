#ifndef INPUTCONDITIONWIDGETCGNSBASEITERATIVEREALRESULT_H
#define INPUTCONDITIONWIDGETCGNSBASEITERATIVEREALRESULT_H

#include "inputconditionwidgetcgnsresult.h"

class InputConditionWidgetCgnsBaseIterativeRealResult : public InputConditionWidgetCgnsResult
{
public:
	InputConditionWidgetCgnsBaseIterativeRealResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file);

private:
	QStringList getEnums() const override;
};

#endif // INPUTCONDITIONWIDGETCGNSBASEITERATIVEREALRESULT_H
