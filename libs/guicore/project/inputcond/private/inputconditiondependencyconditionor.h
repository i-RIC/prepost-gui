#ifndef INPUTCONDITIONDEPENDENCYCONDITIONOR_H
#define INPUTCONDITIONDEPENDENCYCONDITIONOR_H

#include "../inputconditiondependency.h"

#include <vector>

class InputConditionDependencyConditionOr : public InputConditionDependency::Condition
{
public:
	InputConditionDependencyConditionOr(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep);
	~InputConditionDependencyConditionOr();

	bool match();

private:
	std::vector<InputConditionDependency::Condition*> m_children;

};

#endif // INPUTCONDITIONDEPENDENCYCONDITIONOR_H
