#include "inputconditioncgnsfile.h"
#include "inputconditionwidgetcgnsgridcellintegerresult.h"

InputConditionWidgetCgnsGridCellIntegerResult::InputConditionWidgetCgnsGridCellIntegerResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file) :
	InputConditionWidgetCgnsResult {defnode, t, cont, file}
{}

QStringList InputConditionWidgetCgnsGridCellIntegerResult::getEnums() const
{
	return cgnsFile()->gridCellIntegerResults();
}
