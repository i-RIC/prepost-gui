#ifndef INPUTCONDITIONDEPENDENCYCONDITIONISGREATEREQUAL_H
#define INPUTCONDITIONDEPENDENCYCONDITIONISGREATEREQUAL_H

#include "../inputconditiondependency.h"

#include <QVariant>

class InputConditionDependencyConditionIsGreaterEqual : public InputConditionDependency::Condition
{
public:
	InputConditionDependencyConditionIsGreaterEqual(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep);
	~InputConditionDependencyConditionIsGreaterEqual();

	bool match();

private:
	InputConditionContainer* m_target;
	QVariant m_value;
};

#endif // INPUTCONDITIONDEPENDENCYCONDITIONISGREATEREQUAL_H
