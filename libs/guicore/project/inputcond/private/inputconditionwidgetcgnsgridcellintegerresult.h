#ifndef INPUTCONDITIONWIDGETCGNSGRIDCELLINTEGERRESULT_H
#define INPUTCONDITIONWIDGETCGNSGRIDCELLINTEGERRESULT_H

#include "inputconditionwidgetcgnsresult.h"

class InputConditionWidgetCgnsGridCellIntegerResult : public InputConditionWidgetCgnsResult
{
public:
	InputConditionWidgetCgnsGridCellIntegerResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file);

private:
	QStringList getEnums() const override;
};

#endif // INPUTCONDITIONWIDGETCGNSGRIDCELLINTEGERRESULT_H
