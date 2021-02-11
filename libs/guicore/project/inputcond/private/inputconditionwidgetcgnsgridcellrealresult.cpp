#include "inputconditioncgnsfile.h"
#include "inputconditionwidgetcgnsgridcellrealresult.h"

InputConditionWidgetCgnsGridCellRealResult::InputConditionWidgetCgnsGridCellRealResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file) :
	InputConditionWidgetCgnsResult {defnode, t, cont, file}
{}

QStringList InputConditionWidgetCgnsGridCellRealResult::getEnums() const
{
	return cgnsFile()->gridCellRealResults();
}
