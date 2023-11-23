#include "inputconditioncgnsfile.h"
#include "inputconditionwidgetcgnsgridnoderealresult.h"

InputConditionWidgetCgnsGridNodeRealResult::InputConditionWidgetCgnsGridNodeRealResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file, bool noStretch) :
	InputConditionWidgetCgnsResult {defnode, t, cont, file, noStretch}
{}

QStringList InputConditionWidgetCgnsGridNodeRealResult::getEnums() const
{
	return cgnsFile()->gridNodeRealResults();
}
