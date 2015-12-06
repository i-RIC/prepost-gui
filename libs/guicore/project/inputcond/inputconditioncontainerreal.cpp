#include "inputconditioncontainerreal.h"

#include <misc/stringtool.h>

#include <QDomElement>
#include <QDomNode>
#include <QTextStream>

#include <yaml-cpp/yaml.h>

#include <iriclib.h>

InputConditionContainerReal::InputConditionContainerReal() :
	InputConditionContainer(),
	m_value {0},
	m_default {0}
{}

InputConditionContainerReal::InputConditionContainerReal(const QString& n, const QString& c, const QDomNode& defNode) :
	InputConditionContainer(n, c)
{
	setup(defNode);
}

InputConditionContainerReal::InputConditionContainerReal(const InputConditionContainerReal& i) :
	InputConditionContainer(i)
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
	return m_value;
}

void InputConditionContainerReal::setValue(double v)
{
	if (m_value != v) {
		m_value = v;
		emit valueChanged(m_value);
		emit valueChanged();
	}
}

double InputConditionContainerReal::defaultValue() const
{
	return m_default;
}

void InputConditionContainerReal::setDefaultValue(double d)
{
	m_default = d;
}

int InputConditionContainerReal::load()
{
	int ret;
	if (isBoundaryCondition()) {
		ret = cg_iRIC_Read_BC_Real(toC(bcName()), bcIndex(), toC(name()), &m_value);
	} else if (isComplexCondition()) {
		ret = cg_iRIC_Read_Complex_Real(toC(complexName()), complexIndex(), toC(name()), &m_value);
	} else {
		ret = cg_iRIC_Read_Real(toC(name()), &m_value);
	}
	if (ret != 0) {
		clear();
	} else {
		emit valueChanged(m_value);
		emit valueChanged();
	}
	return ret;
}

int InputConditionContainerReal::save()
{
	if (isBoundaryCondition()) {
		return cg_iRIC_Write_BC_Real(toC(bcName()), bcIndex(), toC(name()), m_value);
	} else if (isComplexCondition()) {
		return cg_iRIC_Write_Complex_Real(toC(complexName()), complexIndex(), toC(name()), m_value);
	} else {
		return cg_iRIC_Write_Real(toC(name()), m_value);
	}
}

void InputConditionContainerReal::clear()
{
	if (m_default != m_value) {
		m_value = m_default;
		emit valueChanged(m_value);
		emit valueChanged();
	}
}

QVariant InputConditionContainerReal::variantValue() const
{
	return QVariant(m_value);
}

void InputConditionContainerReal::importFromYaml(const YAML::Node& doc, const QDir&)
{
	if (doc[name()]) {
		m_value = doc[name()].as<double>();
		emit valueChanged(m_value);
		emit valueChanged();
	}
}

void InputConditionContainerReal::exportToYaml(QTextStream* stream, const QDir&)
{
	*stream << name().c_str() << ": " << m_value << "\t#[real] " << caption() << "\r\n";
}

void InputConditionContainerReal::setup(const QDomNode& defNode)
{
	QDomElement e = defNode.toElement();
	m_default = e.attribute("default", "0").toDouble();
	m_value = m_default;
}

void InputConditionContainerReal::copyValues(const InputConditionContainerReal& i)
{
	InputConditionContainer::copyValues(i);
	m_value = i.m_value;
	m_default = i.m_default;
}
