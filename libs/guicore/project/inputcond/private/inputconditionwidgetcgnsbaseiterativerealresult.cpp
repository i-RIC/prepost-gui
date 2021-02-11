#include "inputconditioncgnsfile.h"
#include "inputconditionwidgetcgnsbaseiterativerealresult.h"

InputConditionWidgetCgnsBaseIterativeRealResult::InputConditionWidgetCgnsBaseIterativeRealResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file) :
	InputConditionWidgetCgnsResult {defnode, t, cont, file}
{}

QStringList InputConditionWidgetCgnsBaseIterativeRealResult::getEnums() const
{
	return cgnsFile()->baseIterativeRealResults();
}
