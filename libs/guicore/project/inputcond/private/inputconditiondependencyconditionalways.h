#ifndef INPUTCONDITIONDEPENDENCYCONDITIONALWAYS_H
#define INPUTCONDITIONDEPENDENCYCONDITIONALWAYS_H

#include "../inputconditiondependency.h"

class InputConditionDependencyConditionAlways : public InputConditionDependency::Condition
{
public:
	InputConditionDependencyConditionAlways(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep);
	~InputConditionDependencyConditionAlways();

	bool match();
};

#endif // INPUTCONDITIONDEPENDENCYCONDITIONALWAYS_H
