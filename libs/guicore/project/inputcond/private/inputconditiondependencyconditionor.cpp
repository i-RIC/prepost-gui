#include "../inputconditioncontainerset.h"
#include "inputconditiondependencyconditionor.h"

#include <misc/stringtool.h>

InputConditionDependencyConditionOr::InputConditionDependencyConditionOr(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep) :
	InputConditionDependency::Condition(dep)
{
	const QDomNodeList& list = node.childNodes();
	for (int i = 0; i < list.count(); ++i) {
		InputConditionDependency::Condition* child = InputConditionDependency::buildCondition(list.item(i), cs, dep);
		m_children.push_back(child);
	}
}

InputConditionDependencyConditionOr::~InputConditionDependencyConditionOr()
{
	for (auto c : m_children) {
		delete c;
	}
}

bool InputConditionDependencyConditionOr::match()
{
	bool ret = false;
	for (auto c : m_children) {
		ret = ret || c->match();
	}
	return ret;
}
