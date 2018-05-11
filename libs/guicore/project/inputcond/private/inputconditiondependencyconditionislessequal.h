#ifndef INPUTCONDITIONDEPENDENCYCONDITIONISLESSEQUAL_H
#define INPUTCONDITIONDEPENDENCYCONDITIONISLESSEQUAL_H

#include "../inputconditiondependency.h"

#include <QVariant>

class InputConditionDependencyConditionIsLessEqual : public InputConditionDependency::Condition
{
public:
	InputConditionDependencyConditionIsLessEqual(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep);
	~InputConditionDependencyConditionIsLessEqual();

	bool match();

private:
	InputConditionContainer* m_target;
	QVariant m_value;
};

#endif // INPUTCONDITIONDEPENDENCYCONDITIONISLESSEQUAL_H
