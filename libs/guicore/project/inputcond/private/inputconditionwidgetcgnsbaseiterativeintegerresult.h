#ifndef INPUTCONDITIONWIDGETCGNSBASEITERATIVEINTEGERRESULT_H
#define INPUTCONDITIONWIDGETCGNSBASEITERATIVEINTEGERRESULT_H

#include "inputconditionwidgetcgnsresult.h"

class InputConditionWidgetCgnsBaseIterativeIntegerResult : public InputConditionWidgetCgnsResult
{
public:
	InputConditionWidgetCgnsBaseIterativeIntegerResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file, bool noStretch);

private:
	QStringList getEnums() const override;
};

#endif // INPUTCONDITIONWIDGETCGNSBASEITERATIVEINTEGERRESULT_H
