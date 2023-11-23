#include "inputconditioncgnsfile.h"
#include "inputconditionwidgetcgnsgridedgejintegerresult.h"

InputConditionWidgetCgnsGridEdgeJIntegerResult::InputConditionWidgetCgnsGridEdgeJIntegerResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file, bool noStretch) :
	InputConditionWidgetCgnsResult {defnode, t, cont, file, noStretch}
{}

QStringList InputConditionWidgetCgnsGridEdgeJIntegerResult::getEnums() const
{
	return cgnsFile()->gridEdgeJIntegerResults();
}
