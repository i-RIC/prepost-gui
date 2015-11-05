#include "inputconditioncontainer.h"

InputConditionContainer::InputConditionContainer() :
	InputConditionContainer("", "")
{}

InputConditionContainer::InputConditionContainer(const QString& name, const QString& caption) :
	QObject(nullptr),
	m_name {name},
	m_caption {caption},
	m_isBoundaryCondition {false},
	m_isComplexCondition {false}
{}

InputConditionContainer::InputConditionContainer(const InputConditionContainer& c) :
	QObject(nullptr),
	m_name {c.m_name},
	m_caption {c.m_caption},
	m_isBoundaryCondition {false},
	m_isComplexCondition {false}
{}

void InputConditionContainer::setBCProperty(const QString& bcname, int bcindex)
{
	m_bcName = bcname;
	m_bcIndex = bcindex;
	m_isBoundaryCondition = true;
	m_isComplexCondition = false;
}

void InputConditionContainer::setComplexProperty(const QString& compname, int compindex)
{
	m_complexName = compname;
	m_complexIndex = compindex;
	m_isBoundaryCondition = false;
	m_isComplexCondition = true;
}

void InputConditionContainer::setName(const QString& name)
{
	m_name = name;
}

void InputConditionContainer::copyValues(const InputConditionContainer& c)
{
	m_name = c.m_name;
	m_caption = c.m_caption;
}

const QString& InputConditionContainer::name() const
{
	return m_name;
}

const QString& InputConditionContainer::bcName() const
{
	return m_bcName;
}

const QString& InputConditionContainer::caption() const
{
	return m_caption;
}
