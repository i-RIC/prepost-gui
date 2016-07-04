#include "../inputconditioncontainerset.h"
#include "inputconditiondependencyconditionalways.h"

#include <misc/stringtool.h>

InputConditionDependencyConditionAlways::InputConditionDependencyConditionAlways(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep) :
	InputConditionDependency::Condition(dep)
{
	std::string targName = iRIC::toStr(node.toElement().attribute("target"));
	InputConditionContainer* target = cs->container(targName);
	QObject::connect(target, SIGNAL(valueChanged()), dep, SLOT(check()));
}

InputConditionDependencyConditionAlways::~InputConditionDependencyConditionAlways()
{}

bool InputConditionDependencyConditionAlways::match()
{
	return true;
}
