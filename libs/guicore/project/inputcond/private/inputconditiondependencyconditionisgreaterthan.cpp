#include "../inputconditioncontainerset.h"
#include "inputconditiondependencyconditionisgreaterthan.h"

#include <misc/stringtool.h>

InputConditionDependencyConditionIsGreaterThan::InputConditionDependencyConditionIsGreaterThan(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep) :
	InputConditionDependency::Condition(dep)
{
	QDomElement elem = node.toElement();
	std::string targName = iRIC::toStr(elem.attribute("target"));
	QString valueStr = elem.attribute("value");
	m_target = cs->container(targName);
	m_value = QVariant(valueStr);
	// signal/slot connection
	QObject::connect(m_target, SIGNAL(valueChanged()), dep, SLOT(check()));
}

InputConditionDependencyConditionIsGreaterThan::~InputConditionDependencyConditionIsGreaterThan()
{}

bool InputConditionDependencyConditionIsGreaterThan::match()
{
	QVariant currentValue = m_target->variantValue();
	return currentValue.toDouble() > m_value.toDouble();
}
