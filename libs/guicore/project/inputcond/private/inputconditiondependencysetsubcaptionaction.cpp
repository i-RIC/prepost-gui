#include "inputconditiondependencysetsubcaptionaction.h"
#include "inputconditionwidgetlabel.h"

InputConditionDependencySetSubCaptionAction::InputConditionDependencySetSubCaptionAction(InputConditionWidgetLabel* w, const QString& caption) :
	InputConditionDependency::Action(w),
	m_caption {caption}
{}

void InputConditionDependencySetSubCaptionAction::positiveAction()
{
	auto label = dynamic_cast<InputConditionWidgetLabel*> (m_target);
	label->setCaption(m_caption);
}

void InputConditionDependencySetSubCaptionAction::negativeAction()
{}
