#include "inputconditiondependencychecksubimages.h"
#include "inputconditionwidgetimage.h"

InputConditionDependencyCheckSubImages::InputConditionDependencyCheckSubImages(InputConditionWidgetImage* image) :
	InputConditionDependency(),
	m_image (image)
{}

void InputConditionDependencyCheckSubImages::check()
{
	m_image->inactivateSubImages();

	for (auto dep : m_image->dependencies()) {
		dep->check();
	}
}
