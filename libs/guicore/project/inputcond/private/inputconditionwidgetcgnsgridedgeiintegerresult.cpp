#include "inputconditioncgnsfile.h"
#include "inputconditionwidgetcgnsgridedgeiintegerresult.h"

InputConditionWidgetCgnsGridEdgeIIntegerResult::InputConditionWidgetCgnsGridEdgeIIntegerResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file, bool noStretch) :
	InputConditionWidgetCgnsResult {defnode, t, cont, file, noStretch}
{}

QStringList InputConditionWidgetCgnsGridEdgeIIntegerResult::getEnums() const
{
	return cgnsFile()->gridEdgeIIntegerResults();
}
