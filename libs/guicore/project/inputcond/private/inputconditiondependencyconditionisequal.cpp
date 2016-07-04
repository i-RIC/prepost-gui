#include "../inputconditioncontainerset.h"
#include "inputconditiondependencyconditionisequal.h"

#include <misc/errormessage.h>
#include <misc/stringtool.h>

InputConditionDependencyConditionIsEqual::InputConditionDependencyConditionIsEqual(const QDomNode& node, InputConditionContainerSet* cs, InputConditionDependency* dep) :
	InputConditionDependency::Condition(dep)
{
	QDomElement elem = node.toElement();
	std::string targName = iRIC::toStr(elem.attribute("target"));
	QString valueStr = elem.attribute("value");
	m_target = cs->container(targName);
	if (m_target == 0) {
		// target is not correctly specified.
		throw ErrorMessage("target is not correctly specified.");
	}
	m_value = QVariant(valueStr);

	// signal/slot connection
	QObject::connect(m_target, SIGNAL(valueChanged()), dep, SLOT(check()));
}

InputConditionDependencyConditionIsEqual::~InputConditionDependencyConditionIsEqual()
{}

bool InputConditionDependencyConditionIsEqual::match()
{
	QVariant currentValue = m_target->variantValue();
	return currentValue == m_value;
}
