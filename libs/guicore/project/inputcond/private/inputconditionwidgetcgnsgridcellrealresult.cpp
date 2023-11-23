#include "inputconditioncgnsfile.h"
#include "inputconditionwidgetcgnsgridcellrealresult.h"

InputConditionWidgetCgnsGridCellRealResult::InputConditionWidgetCgnsGridCellRealResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file, bool noStretch) :
	InputConditionWidgetCgnsResult {defnode, t, cont, file, noStretch}
{}

QStringList InputConditionWidgetCgnsGridCellRealResult::getEnums() const
{
	return cgnsFile()->gridCellRealResults();
}
