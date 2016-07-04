#ifndef INPUTCONDITIONDEPENDENCYCONDITIONISEQUAL_H
#define INPUTCONDITIONDEPENDENCYCONDITIONISEQUAL_H

#include "../inputconditiondependency.h"

class InputConditionDependencyConditionIsEqual : public InputConditionDependency::Condition
{
public:
	InputConditionDependencyConditionIsEqual(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep);
	~InputConditionDependencyConditionIsEqual();

	bool match();

private:
	InputConditionContainer* m_target;
	QVariant m_value;
};

#endif // INPUTCONDITIONDEPENDENCYCONDITIONISEQUAL_H
