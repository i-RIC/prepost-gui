#include "inputconditioncontainerinteger.h"

#include <misc/stringtool.h>

#include <QDomNode>
#include <QTextStream>

#include <yaml-cpp/yaml.h>

#include <iriclib.h>

InputConditionContainerInteger::InputConditionContainerInteger() :
	InputConditionContainer(),
	m_default {0},
	m_value {0}
{}

InputConditionContainerInteger::InputConditionContainerInteger(const QString& n, const QString& c, const QDomNode& defNode) :
	InputConditionContainer(n, c)
{
	setup(defNode);
}

InputConditionContainerInteger::InputConditionContainerInteger(const InputConditionContainerInteger& i) :
	InputConditionContainer(i)
{
	copyValues(i);
}

InputConditionContainerInteger& InputConditionContainerInteger::operator=(const InputConditionContainerInteger& i)
{
	copyValues(i);
	return *this;
}

void InputConditionContainerInteger::setup(const QDomNode& defNode)
{
	QDomElement e = defNode.toElement();
	m_default = e.attribute("default", "0").toInt();
	m_value = m_default;
}

void InputConditionContainerInteger::copyValues(const InputConditionContainerInteger& i)
{
	InputConditionContainer::copyValues(i);
	m_value = i.m_value;
	m_default = i.m_default;
}

void InputConditionContainerInteger::clear()
{
	if (m_value != m_default) {
		m_value = m_default;
		emit valueChanged(m_value);
		emit valueChanged();
	}
}

int InputConditionContainerInteger::load()
{
	int ret;
	if (m_isBoundaryCondition) {
		ret = cg_iRIC_Read_BC_Integer(const_cast<char*>(iRIC::toStr(bcName()).c_str()), m_bcIndex, const_cast<char*>(iRIC::toStr(name()).c_str()), &m_value);
	} else if (m_isComplexCondition) {
		ret = cg_iRIC_Read_Complex_Integer(const_cast<char*>(iRIC::toStr(m_complexName).c_str()), m_complexIndex, const_cast<char*>(iRIC::toStr(name()).c_str()), &m_value);
	} else {
		ret = cg_iRIC_Read_Integer(const_cast<char*>(iRIC::toStr(name()).c_str()), &m_value);
	}
	if (ret != 0) {
		clear();
	} else {
		emit valueChanged(m_value);
		emit valueChanged();
	}
	return ret;
}

int InputConditionContainerInteger::save()
{
	if (m_isBoundaryCondition) {
		return cg_iRIC_Write_BC_Integer(const_cast<char*>(iRIC::toStr(bcName()).c_str()), m_bcIndex, const_cast<char*>(iRIC::toStr(name()).c_str()), m_value);
	} else if (m_isComplexCondition) {
		return cg_iRIC_Write_Complex_Integer(const_cast<char*>(iRIC::toStr(m_complexName).c_str()), m_complexIndex, const_cast<char*>(iRIC::toStr(name()).c_str()), m_value);
	} else {
		return cg_iRIC_Write_Integer(const_cast<char*>(iRIC::toStr(name()).c_str()), m_value);
	}
}

QVariant InputConditionContainerInteger::variantValue() const
{
	return QVariant(m_value);
}

void InputConditionContainerInteger::setValue(int v)
{
	if (m_value != v) {
		m_value = v;
		emit valueChanged(m_value);
		emit valueChanged();
	}
}

int InputConditionContainerInteger::value() const
{
	return m_value;
}

void InputConditionContainerInteger::setDefaultValue(int d)
{
	m_default = d;
}

int InputConditionContainerInteger::defaultValue() const
{
	return m_default;
}

void InputConditionContainerInteger::importFromYaml(const YAML::Node& doc, const QDir&)
{
	if (doc[iRIC::toStr(name()).c_str()]) {
		m_value = doc[iRIC::toStr(name()).c_str()].as<int>();
		emit valueChanged(m_value);
		emit valueChanged();
	}
}

void InputConditionContainerInteger::exportToYaml(QTextStream* stream, const QDir&)
{
	*stream << name() << ": " << m_value << "\t#[integer] " << caption() << "\r\n";
}
