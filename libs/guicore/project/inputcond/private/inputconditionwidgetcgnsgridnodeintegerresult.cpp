#include "inputconditioncgnsfile.h"
#include "inputconditionwidgetcgnsgridnodeintegerresult.h"

InputConditionWidgetCgnsGridNodeIntegerResult::InputConditionWidgetCgnsGridNodeIntegerResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file) :
	InputConditionWidgetCgnsResult {defnode, t, cont, file}
{}

QStringList InputConditionWidgetCgnsGridNodeIntegerResult::getEnums() const
{
	return cgnsFile()->gridNodeIntegerResults();
}
