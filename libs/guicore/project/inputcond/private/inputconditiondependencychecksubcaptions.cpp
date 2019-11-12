#include "inputconditiondependencychecksubcaptions.h"
#include "inputconditionwidgetlabel.h"

InputConditionDependencyCheckSubCaptions::InputConditionDependencyCheckSubCaptions(InputConditionWidgetLabel* label) :
	InputConditionDependency(),
	m_label {label}
{}

void InputConditionDependencyCheckSubCaptions::check()
{
	m_label->restoreCaption();

	for (auto dep : m_label->dependencies()) {
		dep->check();
	}
}
