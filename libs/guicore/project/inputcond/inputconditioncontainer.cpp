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

InputConditionContainer::~InputConditionContainer()
{}

void InputConditionContainer::setName(const QString& name)
{
	m_name = name;
}

void InputConditionContainer::setBCProperty(const QString& bcname, int bcindex)
{
	m_isBoundaryCondition = true;
	m_bcName = bcname;
	m_bcIndex = bcindex;

	m_isComplexCondition = false;
}

void InputConditionContainer::setComplexProperty(const QString& compname, int compindex)
{
	m_isComplexCondition = true;
	m_complexName = compname;
	m_complexIndex = compindex;

	m_isBoundaryCondition = false;
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

const QString& InputConditionContainer::caption() const
{
	return m_caption;
}

bool InputConditionContainer::isBoundaryCondition() const
{
	return m_isBoundaryCondition;
}

const QString& InputConditionContainer::bcName() const
{
	return m_bcName;
}

int InputConditionContainer::bcIndex() const
{
	return m_bcIndex;
}

bool InputConditionContainer::isComplexCondition() const
{
	return m_isComplexCondition;
}

const QString& InputConditionContainer::complexName() const
{
	return m_complexName;
}

int InputConditionContainer::complexIndex() const
{
	return m_complexIndex;
}
