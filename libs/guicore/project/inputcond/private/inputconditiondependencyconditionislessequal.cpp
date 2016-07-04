#include "../inputconditioncontainerset.h"
#include "inputconditiondependencyconditionislessequal.h"

#include <misc/stringtool.h>

InputConditionDependencyConditionIsLessEqual::InputConditionDependencyConditionIsLessEqual(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep) :
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

InputConditionDependencyConditionIsLessEqual::~InputConditionDependencyConditionIsLessEqual()
{}

bool InputConditionDependencyConditionIsLessEqual::match()
{
	QVariant currentValue = m_target->variantValue();
	return currentValue.toDouble() <= m_value.toDouble();
}
