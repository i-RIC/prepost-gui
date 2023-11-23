#include "inputconditioncgnsfile.h"
#include "inputconditionwidgetcgnsbaseiterativerealresult.h"

InputConditionWidgetCgnsBaseIterativeRealResult::InputConditionWidgetCgnsBaseIterativeRealResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file, bool noStretch) :
	InputConditionWidgetCgnsResult {defnode, t, cont, file, noStretch}
{}

QStringList InputConditionWidgetCgnsBaseIterativeRealResult::getEnums() const
{
	return cgnsFile()->baseIterativeRealResults();
}
