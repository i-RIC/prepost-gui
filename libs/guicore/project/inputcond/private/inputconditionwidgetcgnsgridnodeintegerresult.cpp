#include "inputconditioncgnsfile.h"
#include "inputconditionwidgetcgnsgridnodeintegerresult.h"

InputConditionWidgetCgnsGridNodeIntegerResult::InputConditionWidgetCgnsGridNodeIntegerResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file, bool noStretch) :
	InputConditionWidgetCgnsResult {defnode, t, cont, file, noStretch}
{}

QStringList InputConditionWidgetCgnsGridNodeIntegerResult::getEnums() const
{
	return cgnsFile()->gridNodeIntegerResults();
}
