#ifndef INPUTCONDITIONWIDGETCGNSGRIDCELLREALRESULT_H
#define INPUTCONDITIONWIDGETCGNSGRIDCELLREALRESULT_H

#include "inputconditionwidgetcgnsresult.h"

class InputConditionWidgetCgnsGridCellRealResult : public InputConditionWidgetCgnsResult
{
public:
	InputConditionWidgetCgnsGridCellRealResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file, bool noStretch);

private:
	QStringList getEnums() const override;
};

#endif // INPUTCONDITIONWIDGETCGNSGRIDCELLREALRESULT_H
