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

InputConditionContainerReal& InputConditionContainerReal::operator=(const InputConditionContainerReal& i)
{
	copyValues(i);
	return *this;
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

QVariant InputConditionContainerReal::variantValue() const
{
	return QVariant(m_value);
}

void InputConditionContainerReal::setValue(double v)
{
	if (m_value != v) {
		m_value = v;
		emit valueChanged(m_value);
		emit valueChanged();
	}
}

double InputConditionContainerReal::value() const
{
	return m_value;
}

void InputConditionContainerReal::setDefaultValue(double d)
{
	m_default = d;
}

double InputConditionContainerReal::defaultValue() const
{
	return m_default;
}

void InputConditionContainerReal::clear()
{
	if (m_default != m_value) {
		m_value = m_default;
		emit valueChanged(m_value);
		emit valueChanged();
	}
}

int InputConditionContainerReal::load()
{
	int ret;
	if (m_isBoundaryCondition) {
		ret = cg_iRIC_Read_BC_Real(const_cast<char*>(iRIC::toStr(bcName()).c_str()), m_bcIndex, const_cast<char*>(iRIC::toStr(name()).c_str()), &m_value);
	} else if (m_isComplexCondition) {
		ret = cg_iRIC_Read_Complex_Real(const_cast<char*>(iRIC::toStr(m_complexName).c_str()), m_complexIndex, const_cast<char*>(iRIC::toStr(name()).c_str()), &m_value);
	} else {
		ret = cg_iRIC_Read_Real(const_cast<char*>(iRIC::toStr(name()).c_str()), &m_value);
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
	if (m_isBoundaryCondition) {
		return cg_iRIC_Write_BC_Real(const_cast<char*>(iRIC::toStr(bcName()).c_str()), m_bcIndex, const_cast<char*>(iRIC::toStr(name()).c_str()), m_value);
	} else if (m_isComplexCondition) {
		return cg_iRIC_Write_Complex_Real(const_cast<char*>(iRIC::toStr(m_complexName).c_str()), m_complexIndex, const_cast<char*>(iRIC::toStr(name()).c_str()), m_value);
	} else {
		return cg_iRIC_Write_Real(const_cast<char*>(iRIC::toStr(name()).c_str()), m_value);
	}
}

void InputConditionContainerReal::importFromYaml(const YAML::Node& doc, const QDir&)
{
	if (doc[iRIC::toStr(name()).c_str()]) {
		m_value = doc[iRIC::toStr(name()).c_str()].as<double>();
		emit valueChanged(m_value);
		emit valueChanged();
	}
}

void InputConditionContainerReal::exportToYaml(QTextStream* stream, const QDir&)
{
	*stream << name() << ": " << m_value << "\t#[real] " << caption() << "\r\n";
}
