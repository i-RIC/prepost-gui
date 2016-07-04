#include "inputconditiondependencyconditionnot.h"

InputConditionDependencyConditionNot::InputConditionDependencyConditionNot(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep) :
	InputConditionDependency::Condition(dep)
{
	m_child = nullptr;
	const QDomNodeList& list = node.childNodes();
	for (int i = 0; i < list.count(); ++i) {
		m_child = InputConditionDependency::buildCondition(list.item(i), cs, dep);
		// use the first child only.
		break;
	}
}

InputConditionDependencyConditionNot::~InputConditionDependencyConditionNot()
{
	delete m_child;
}

bool InputConditionDependencyConditionNot::match()
{
	return ! m_child->match();
}
