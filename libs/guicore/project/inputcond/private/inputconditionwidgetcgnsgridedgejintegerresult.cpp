#include "inputconditioncgnsfile.h"
#include "inputconditionwidgetcgnsgridedgejintegerresult.h"

InputConditionWidgetCgnsGridEdgeJIntegerResult::InputConditionWidgetCgnsGridEdgeJIntegerResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file) :
	InputConditionWidgetCgnsResult {defnode, t, cont, file}
{}

QStringList InputConditionWidgetCgnsGridEdgeJIntegerResult::getEnums() const
{
	return cgnsFile()->gridEdgeJIntegerResults();
}
