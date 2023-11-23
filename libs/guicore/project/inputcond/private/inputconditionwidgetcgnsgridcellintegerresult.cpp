#include "inputconditioncgnsfile.h"
#include "inputconditionwidgetcgnsgridcellintegerresult.h"

InputConditionWidgetCgnsGridCellIntegerResult::InputConditionWidgetCgnsGridCellIntegerResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file, bool noStretch) :
	InputConditionWidgetCgnsResult {defnode, t, cont, file, noStretch}
{}

QStringList InputConditionWidgetCgnsGridCellIntegerResult::getEnums() const
{
	return cgnsFile()->gridCellIntegerResults();
}
