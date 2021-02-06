#include "inputconditioncgnsfile.h"
#include "inputconditionwidgetcgnsgridedgeirealresult.h"

InputConditionWidgetCgnsGridEdgeIRealResult::InputConditionWidgetCgnsGridEdgeIRealResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file) :
	InputConditionWidgetCgnsResult {defnode, t, cont, file}
{}

QStringList InputConditionWidgetCgnsGridEdgeIRealResult::getEnums() const
{
	return cgnsFile()->gridEdgeIRealResults();
}
