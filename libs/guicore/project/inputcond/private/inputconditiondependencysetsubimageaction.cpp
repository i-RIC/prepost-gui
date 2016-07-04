#include "inputconditiondependencysetsubimageaction.h"
#include "inputconditionwidgetimage.h"

InputConditionDependencySetSubimageAction::InputConditionDependencySetSubimageAction(InputConditionWidgetImage* w, const QString& name) :
	InputConditionDependency::Action(w),
	m_imageName(name)
{}

InputConditionDependencySetSubimageAction::~InputConditionDependencySetSubimageAction()
{}

void InputConditionDependencySetSubimageAction::positiveAction()
{
	InputConditionWidgetImage* image = dynamic_cast<InputConditionWidgetImage*> (m_target);
	image->activateSubImage(m_imageName);
}

void InputConditionDependencySetSubimageAction::negativeAction()
{}
