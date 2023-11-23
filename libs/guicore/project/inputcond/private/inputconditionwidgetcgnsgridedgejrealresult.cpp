#include "inputconditioncgnsfile.h"
#include "inputconditionwidgetcgnsgridedgejrealresult.h"

InputConditionWidgetCgnsGridEdgeJRealResult::InputConditionWidgetCgnsGridEdgeJRealResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file, bool noStretch) :
	InputConditionWidgetCgnsResult {defnode, t, cont, file, noStretch}
{}

QStringList InputConditionWidgetCgnsGridEdgeJRealResult::getEnums() const
{
	return cgnsFile()->gridEdgeJRealResults();
}
