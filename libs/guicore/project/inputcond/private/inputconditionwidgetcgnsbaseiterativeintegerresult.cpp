#include "inputconditioncgnsfile.h"
#include "inputconditionwidgetcgnsbaseiterativeintegerresult.h"

InputConditionWidgetCgnsBaseIterativeIntegerResult::InputConditionWidgetCgnsBaseIterativeIntegerResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file) :
	InputConditionWidgetCgnsResult {defnode, t, cont, file}
{}

QStringList InputConditionWidgetCgnsBaseIterativeIntegerResult::getEnums() const
{
	return cgnsFile()->baseIterativeIntegerResults();
}
