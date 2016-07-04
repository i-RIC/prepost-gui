#include "inputconditiondependencychecksubimages.h"
#include "inputconditionwidgetimage.h"

InputConditionDependencyCheckSubimages::InputConditionDependencyCheckSubimages(InputConditionWidgetImage* image) :
	InputConditionDependency(),
	m_image (image)
{}

void InputConditionDependencyCheckSubimages::check()
{
	m_image->inactivateSubImages();

	const std::vector<InputConditionDependency*>& deps = m_image->dependencies();
	std::vector<InputConditionDependency*>::const_iterator it;

	for (it = deps.begin(); it != deps.end(); ++it) {
		InputConditionDependency* dep = *it;
		dep->check();
	}
}
