#include "inputconditiondependencychecksubenumerations.h"
#include "inputconditionwidgetintegeroption.h"


InputConditionDependencyCheckSubEnumerations::InputConditionDependencyCheckSubEnumerations(InputConditionWidgetIntegerOption* integerOption) :
	InputConditionDependency(),
	m_integerOption(integerOption)
{}

void InputConditionDependencyCheckSubEnumerations::check()
{
	m_integerOption->inactivateSubEnumerations();

	for (auto dep : m_integerOption->dependencies()) {
		dep->check();
	}
}
