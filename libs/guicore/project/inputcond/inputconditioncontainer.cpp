#include "inputconditioncontainer.h"
#include "private/inputconditioncontainer_impl.h"

#include <misc/stringtool.h>

InputConditionContainer::Impl::Impl(const std::string &name, const QString &caption) :
	m_name (name),
	m_caption (caption)
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

char* InputConditionContainer::toC(const std::string& str)
{
	return const_cast<char*> (str.c_str());
}
