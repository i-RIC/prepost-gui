#include "inputconditioncontainerreal.h"
#include "private/inputconditioncontainerreal_impl.h"

#include <misc/stringtool.h>

#include <QDomElement>
#include <QDomNode>
#include <QTextStream>

#include <yaml-cpp/yaml.h>

#include <h5cgnsconditiongroup.h>

InputConditionContainerReal::Impl::Impl() :
	m_value {0},
	m_default {0}
{}

// public interfaces

InputConditionContainerReal::InputConditionContainerReal() :
	InputConditionContainer(),
	impl {new Impl {}}
{}

InputConditionContainerReal::InputConditionContainerReal(const std::string& n, const QString& c, const QDomNode& defNode) :
	InputConditionContainer(n, c),
	impl {new Impl {}}
{
	setup(defNode);
}

InputConditionContainerReal::InputConditionContainerReal(const InputConditionContainerReal& i) :
	InputConditionContainer(i),
	impl {new Impl {}}
{
	copyValues(i);
}

InputConditionContainerReal::~InputConditionContainerReal()
{}

InputConditionContainerReal& InputConditionContainerReal::operator=(const InputConditionContainerReal& i)
{
	copyValues(i);
	return *this;
}

double InputConditionContainerReal::value() const
{
	return impl->m_value;
}

void InputConditionContainerReal::setValue(double v)
{
	if (impl->m_value != v) {
		impl->m_value = v;
		emit valueChanged(impl->m_value);
		emit valueChanged();
	}
}

double InputConditionContainerReal::defaultValue() const
{
	return impl->m_default;
}

void InputConditionContainerReal::setDefaultValue(double d)
{
	impl->m_default = d;
}

int InputConditionContainerReal::load(const iRICLib::H5CgnsConditionGroup& group)
{
	int ret = group.readRealValueAsDouble(name(), &(impl->m_value));

	if (ret != 0) {
		clear();
	} else {
		emit valueChanged(impl->m_value);
		emit valueChanged();
	}
	return ret;
}

int InputConditionContainerReal::save(iRICLib::H5CgnsConditionGroup* group)
{
	return group->writeRealValue(name(), impl->m_value);
}

void InputConditionContainerReal::clear()
{
	if (impl->m_default != impl->m_value) {
		impl->m_value = impl->m_default;
		emit valueChanged(impl->m_value);
		emit valueChanged();
	}
}

QVariant InputConditionContainerReal::variantValue() const
{
	return QVariant(impl->m_value);
}

void InputConditionContainerReal::importFromYaml(const YAML::Node& doc, const QDir&)
{
	if (doc[name()]) {
		impl->m_value = doc[name()].as<double>();
		emit valueChanged(impl->m_value);
		emit valueChanged();
	}
}

void InputConditionContainerReal::exportToYaml(QTextStream* stream, const QDir&)
{
	*stream << name().c_str() << ": " << impl->m_value << "\t#[real] " << caption() << "\r\n";
}

void InputConditionContainerReal::setup(const QDomNode& defNode)
{
	QDomElement e = defNode.toElement();
	impl->m_default = e.attribute("default", "0").toDouble();
	impl->m_value = impl->m_default;
}

void InputConditionContainerReal::copyValues(const InputConditionContainerReal& i)
{
	InputConditionContainer::copyValues(i);
	setValue(i.value());
	setDefaultValue(i.defaultValue());
}
