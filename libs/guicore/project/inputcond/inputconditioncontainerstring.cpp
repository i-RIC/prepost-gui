#include "inputconditioncontainerstring.h"
#include "private/inputconditioncontainerstring_impl.h"

#include <misc/stringtool.h>

#include <QDomNode>
#include <QTextStream>

#include <yaml-cpp/yaml.h>

#include <h5cgnsconditiongroup.h>
#include <iriclib_errorcodes.h>

InputConditionContainerString::InputConditionContainerString() :
	InputConditionContainer(),
	impl {new Impl {}}
{}

InputConditionContainerString::InputConditionContainerString(const std::string& n, const QString& c, const QDomNode& defNode) :
	InputConditionContainer(n, c),
	impl {new Impl {}}
{
	setup(defNode);
}

InputConditionContainerString::InputConditionContainerString(const InputConditionContainerString& i) :
	InputConditionContainer(i),
	impl {new Impl {}}
{
	copyValues(i);
}

InputConditionContainerString::~InputConditionContainerString()
{
	delete impl;
}

InputConditionContainerString& InputConditionContainerString::operator=(const InputConditionContainerString& i)
{
	copyValues(i);
	return *this;
}

const QString& InputConditionContainerString::value() const
{
	return impl->m_value;
}

void InputConditionContainerString::setValue(const QString& v)
{
	if (impl->m_value != v) {
		impl->m_value = v;
		emit valueChanged(impl->m_value);
		emit valueChanged();
	}
}

const QString& InputConditionContainerString::defaultValue() const
{
	return impl->m_default;
}

void InputConditionContainerString::setDefaultValue(const QString& v)
{
	impl->m_default = v;
}

int InputConditionContainerString::load(const iRICLib::H5CgnsConditionGroup& group)
{
	int length;
	int ret = group.readStringLen(name(), &length);

	if (ret != 0){
		clear();
		return IRIC_NO_ERROR;
	}

	std::vector<char> buffer(length + 1);
	ret = group.readString(name(), buffer.data());

	if (ret != 0){
		clear();
	} else {
		impl->m_value = buffer.data();
		emit valueChanged(impl->m_value);
		emit valueChanged();
	}

	return IRIC_NO_ERROR;
}

int InputConditionContainerString::save(iRICLib::H5CgnsConditionGroup* group)
{
	if (impl->m_value.length() == 0) {return IRIC_NO_ERROR;}

	return group->writeString(name(), iRIC::toStr(impl->m_value));
}

void InputConditionContainerString::clear()
{
	impl->m_value = impl->m_default;
	emit valueChanged(impl->m_value);
}

QVariant InputConditionContainerString::variantValue() const
{
	return QVariant(impl->m_value);
}

void InputConditionContainerString::importFromYaml(const YAML::Node& doc, const QDir&)
{
	if (doc[name()]) {
		impl->m_value = doc[name()].as<std::string>().c_str();
		emit valueChanged(impl->m_value);
		emit valueChanged();
	}
}

void InputConditionContainerString::exportToYaml(QTextStream* stream, const QDir&)
{
	*stream << name().c_str() << ": " << impl->m_value << "\t#[string] " << caption() << "\r\n";
}

void InputConditionContainerString::setup(const QDomNode& defNode)
{
	QDomElement e = defNode.toElement();
	impl->m_default = e.attribute("default", "");
	impl->m_value = impl->m_default;
}

void InputConditionContainerString::copyValues(const InputConditionContainerString& i)
{
	InputConditionContainer::copyValues(i);
	setValue(i.value());
	setDefaultValue(i.defaultValue());
}
