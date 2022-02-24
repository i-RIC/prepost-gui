#include "inputconditiondependencysetsubenumerationsaction.h"

#include "inputconditiondependencysetsubenumerationsaction.h"
#include "inputconditionwidgetintegeroption.h"

InputConditionDependencySetSubEnumerationsAction::InputConditionDependencySetSubEnumerationsAction(InputConditionWidgetIntegerOption* w, const QString& name) :
    InputConditionDependency::Action(w),
    m_name{ name }
{}

InputConditionDependencySetSubEnumerationsAction::~InputConditionDependencySetSubEnumerationsAction()
{}

void InputConditionDependencySetSubEnumerationsAction::positiveAction()
{
    InputConditionWidgetIntegerOption* option = dynamic_cast<InputConditionWidgetIntegerOption*> (m_target);
    Q_ASSERT(option != nullptr);
    option->activateSubEnumerations(m_name);
}

void InputConditionDependencySetSubEnumerationsAction::negativeAction()
{}
