#include "inputconditioncgnsfile.h"
#include "inputconditionwidgetcgnsgridnoderealresult.h"

InputConditionWidgetCgnsGridNodeRealResult::InputConditionWidgetCgnsGridNodeRealResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file) :
	InputConditionWidgetCgnsResult {defnode, t, cont, file}
{}

QStringList InputConditionWidgetCgnsGridNodeRealResult::getEnums() const
{
	return cgnsFile()->gridNodeRealResults();
}
