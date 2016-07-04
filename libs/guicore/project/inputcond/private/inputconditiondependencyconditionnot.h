#ifndef INPUTCONDITIONDEPENDENCYCONDITIONNOT_H
#define INPUTCONDITIONDEPENDENCYCONDITIONNOT_H

#include "../inputconditiondependency.h"

class InputConditionDependencyConditionNot : public InputConditionDependency::Condition
{
public:
	InputConditionDependencyConditionNot(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep);
	~InputConditionDependencyConditionNot();

	bool match();

private:
	InputConditionDependency::Condition* m_child;
};

#endif // INPUTCONDITIONDEPENDENCYCONDITIONNOT_H
