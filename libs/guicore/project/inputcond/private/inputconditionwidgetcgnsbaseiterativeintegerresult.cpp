#include "inputconditioncgnsfile.h"
#include "inputconditionwidgetcgnsbaseiterativeintegerresult.h"

InputConditionWidgetCgnsBaseIterativeIntegerResult::InputConditionWidgetCgnsBaseIterativeIntegerResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file, bool noStretch) :
	InputConditionWidgetCgnsResult {defnode, t, cont, file, noStretch}
{}

QStringList InputConditionWidgetCgnsBaseIterativeIntegerResult::getEnums() const
{
	return cgnsFile()->baseIterativeIntegerResults();
}
