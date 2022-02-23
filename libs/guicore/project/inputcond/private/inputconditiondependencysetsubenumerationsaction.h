#ifndef INPUTCONDITIONDEPENDENCYSETSUBENUMERATIONSACTION_H
#define INPUTCONDITIONDEPENDENCYSETSUBENUMERATIONSACTION_H

#include "../inputconditiondependency.h"

class InputConditionWidgetIntegerOption;


class InputConditionDependencySetSubEnumerationsAction : public InputConditionDependency::Action
{
public:
    InputConditionDependencySetSubEnumerationsAction(InputConditionWidgetIntegerOption* w, const QString& name);
    ~InputConditionDependencySetSubEnumerationsAction();

    void positiveAction();
    void negativeAction();

private:
    QString m_name;
};

#endif // INPUTCONDITIONDEPENDENCYSETSUBENUMERATIONSACTION_H
