#ifndef INPUTCONDITIONDEPENDENCYCONDITIONAND_H
#define INPUTCONDITIONDEPENDENCYCONDITIONAND_H

#include "../inputconditiondependency.h"

#include <vector>

class InputConditionDependencyConditionAnd : public InputConditionDependency::Condition
{
public:
	InputConditionDependencyConditionAnd(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep);
	~InputConditionDependencyConditionAnd();

	bool match();

private:
	std::vector<InputConditionDependency::Condition*> m_children;
};

#endif // INPUTCONDITIONDEPENDENCYCONDITIONAND_H
