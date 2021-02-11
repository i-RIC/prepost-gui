#include "inputconditioncgnsfile.h"
#include "inputconditionwidgetcgnsgridedgeiintegerresult.h"

InputConditionWidgetCgnsGridEdgeIIntegerResult::InputConditionWidgetCgnsGridEdgeIIntegerResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file) :
	InputConditionWidgetCgnsResult {defnode, t, cont, file}
{}

QStringList InputConditionWidgetCgnsGridEdgeIIntegerResult::getEnums() const
{
	return cgnsFile()->gridEdgeIIntegerResults();
}
