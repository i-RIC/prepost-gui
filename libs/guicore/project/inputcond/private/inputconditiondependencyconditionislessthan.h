#ifndef INPUTCONDITIONDEPENDENCYCONDITIONISLESSTHAN_H
#define INPUTCONDITIONDEPENDENCYCONDITIONISLESSTHAN_H

#include "../inputconditiondependency.h"

#include <QVariant>

class InputConditionDependencyConditionIsLessThan : public InputConditionDependency::Condition
{
public:
	InputConditionDependencyConditionIsLessThan(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep);
	~InputConditionDependencyConditionIsLessThan();

	bool match();

private:
	InputConditionContainer* m_target;
	QVariant m_value;
};

#endif // INPUTCONDITIONDEPENDENCYCONDITIONISLESSTHAN_H
