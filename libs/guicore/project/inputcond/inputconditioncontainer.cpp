#include "inputconditioncontainer.h"
#include "private/inputconditioncontainer_impl.h"

#include <misc/stringtool.h>

InputConditionContainer::Impl::Impl(const std::string &name, const QString &caption) :
	m_name (name),
	m_caption (caption),
	m_isBoundaryCondition {false},
	m_isComplexCondition {false}
{}

// public interfaces

InputConditionContainer::InputConditionContainer() :
	InputConditionContainer("", "")
{}

InputConditionContainer::InputConditionContainer(const std::string& name, const QString& caption) :
	QObject(nullptr),
	impl {new Impl {name, caption}}
{}

InputConditionContainer::InputConditionContainer(const InputConditionContainer& c) :
	InputConditionContainer()
{
	*impl = *(c.impl);
}

InputConditionContainer::~InputConditionContainer()
{
	delete impl;
}

void InputConditionContainer::setName(const std::string& name)
{
	impl->m_name = name;
}

void InputConditionContainer::setBCProperty(const std::string& bcname, int bcindex)
{
	impl->m_isBoundaryCondition = true;
	impl->m_bcName = bcname;
	impl->m_bcIndex = bcindex;

	impl->m_isComplexCondition = false;
}

void InputConditionContainer::setComplexProperty(const std::string& compname, int compindex)
{
	impl->m_isComplexCondition = true;
	impl->m_complexName = compname;
	impl->m_complexIndex = compindex;

	impl->m_isBoundaryCondition = false;
}


void InputConditionContainer::copyValues(const InputConditionContainer& c)
{
	impl->m_name = c.impl->m_name;
	impl->m_caption = c.impl->m_caption;
}

const std::string& InputConditionContainer::name() const
{
	return impl->m_name;
}

const QString& InputConditionContainer::caption() const
{
	return impl->m_caption;
}

bool InputConditionContainer::isBoundaryCondition() const
{
	return impl->m_isBoundaryCondition;
}

const std::string& InputConditionContainer::bcName() const
{
	return impl->m_bcName;
}

int InputConditionContainer::bcIndex() const
{
	return impl->m_bcIndex;
}

bool InputConditionContainer::isComplexCondition() const
{
	return impl->m_isComplexCondition;
}

const std::string& InputConditionContainer::complexName() const
{
	return impl->m_complexName;
}

int InputConditionContainer::complexIndex() const
{
	return impl->m_complexIndex;
}

char* InputConditionContainer::toC(const std::string& str)
{
	return const_cast<char*> (str.c_str());
}
