#ifndef INPUTCONDITIONDEPENDENCYCONDITIONISGREATERTHAN_H
#define INPUTCONDITIONDEPENDENCYCONDITIONISGREATERTHAN_H

#include "../inputconditiondependency.h"

#include <QVariant>

class InputConditionDependencyConditionIsGreaterThan : public InputConditionDependency::Condition
{
public:
	InputConditionDependencyConditionIsGreaterThan(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep);
	~InputConditionDependencyConditionIsGreaterThan();

	bool match();

private:
	InputConditionContainer* m_target;
	QVariant m_value;
};

#endif // INPUTCONDITIONDEPENDENCYCONDITIONISGREATERTHAN_H
