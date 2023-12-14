#include "inputconditioncgnsfile.h"
#include "inputconditionwidgetcgnsgridedgeirealresult.h"

InputConditionWidgetCgnsGridEdgeIRealResult::InputConditionWidgetCgnsGridEdgeIRealResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file, bool noStretch) :
	InputConditionWidgetCgnsResult {defnode, t, cont, file, noStretch}
{}

QStringList InputConditionWidgetCgnsGridEdgeIRealResult::getEnums() const
{
	return cgnsFile()->gridEdgeIRealResults();
}
