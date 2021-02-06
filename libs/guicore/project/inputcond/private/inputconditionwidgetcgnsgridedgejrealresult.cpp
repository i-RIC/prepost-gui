#include "inputconditioncgnsfile.h"
#include "inputconditionwidgetcgnsgridedgejrealresult.h"

InputConditionWidgetCgnsGridEdgeJRealResult::InputConditionWidgetCgnsGridEdgeJRealResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file) :
	InputConditionWidgetCgnsResult {defnode, t, cont, file}
{}

QStringList InputConditionWidgetCgnsGridEdgeJRealResult::getEnums() const
{
	return cgnsFile()->gridEdgeJRealResults();
}
