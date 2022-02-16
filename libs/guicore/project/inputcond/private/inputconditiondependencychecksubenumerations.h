#ifndef INPUTCONDITIONDEPENDENCYCHECKSUBENUMERATIONS_H
#define INPUTCONDITIONDEPENDENCYCHECKSUBENUMERATIONS_H

#include "../inputconditiondependency.h"

class InputConditionWidgetIntegerOption;

class InputConditionDependencyCheckSubEnumerations : public InputConditionDependency
{
	Q_OBJECT

public:
	InputConditionDependencyCheckSubEnumerations(InputConditionWidgetIntegerOption* integerOption);

public slots:
	void check() override;

private:
	InputConditionWidgetIntegerOption* m_integerOption;
};

#endif // INPUTCONDITIONDEPENDENCYCHECKSUBENUMERATIONS_H
