#include "inputconditioncontainerstring.h"

#include <misc/stringtool.h>

#include <QDomNode>
#include <QTextStream>

#include <yaml-cpp/yaml.h>

#include <iriclib.h>


InputConditionContainerString::InputConditionContainerString() :
	InputConditionContainer()
{}

InputConditionContainerString::InputConditionContainerString(QString n, const QDomNode& defNode) :
	InputConditionContainer(n)
{
	setup(defNode);
}

InputConditionContainerString::InputConditionContainerString(const InputConditionContainerString& i) :
	InputConditionContainer(i)
{
	copyValues(i);
}

InputConditionContainerString& InputConditionContainerString::operator=(const InputConditionContainerString& i)
{
	copyValues(i);
	return *this;
}

void InputConditionContainerString::clear()
{
	m_value = m_default;
	emit valueChanged(m_value);
}

void InputConditionContainerString::setup(const QDomNode& defNode)
{
	QDomElement e = defNode.toElement();
	m_default = e.attribute("default", "");
	m_value = m_default;
}

int InputConditionContainerString::load()
{
	char buffer[200];
	int ret;
	if (m_isBoundaryCondition) {
		ret = cg_iRIC_Read_BC_String(const_cast<char*>(iRIC::toStr(m_bcName).c_str()), m_bcIndex, const_cast<char*>(iRIC::toStr(m_name).c_str()), buffer);
	} else if (m_isComplexCondition) {
		ret = cg_iRIC_Read_Complex_String(const_cast<char*>(iRIC::toStr(m_complexName).c_str()), m_complexIndex, const_cast<char*>(iRIC::toStr(m_name).c_str()), buffer);
	} else {
		ret = cg_iRIC_Read_String(const_cast<char*>(iRIC::toStr(m_name).c_str()), buffer);
	}
	if (ret != 0) {
		clear();
	} else {
		m_value = buffer;
		emit valueChanged(m_value);
		emit valueChanged();
	}
	return ret;
}

int InputConditionContainerString::save()
{
	std::string value = m_value.toUtf8().constData();
	if (m_isBoundaryCondition) {
		return cg_iRIC_Write_BC_String(const_cast<char*>(iRIC::toStr(m_bcName).c_str()), m_bcIndex, const_cast<char*>(iRIC::toStr(m_name).c_str()), const_cast<char*>(value.c_str()));
	} else if (m_isComplexCondition) {
		return cg_iRIC_Write_Complex_String(const_cast<char*>(iRIC::toStr(m_complexName).c_str()), m_complexIndex, const_cast<char*>(iRIC::toStr(m_name).c_str()), const_cast<char*>(value.c_str()));
	} else {
		return cg_iRIC_Write_String(const_cast<char*>(iRIC::toStr(m_name).c_str()), const_cast<char*>(value.c_str()));
	}
}

QVariant InputConditionContainerString::variantValue() const
{
	return QVariant(m_value);
}

void InputConditionContainerString::setValue(const QString& v)
{
	if (m_value != v) {
		m_value = v;
		emit valueChanged(m_value);
		emit valueChanged();
	}
}

const QString& InputConditionContainerString::value() const {
	return m_value;
}

void InputConditionContainerString::importFromYaml(const YAML::Node& doc)
{
	if (doc[iRIC::toStr(m_name)]) {
		m_value = doc[iRIC::toStr(m_name)].as<std::string>().c_str();
		emit valueChanged(m_value);
		emit valueChanged();
	}
}

void InputConditionContainerString::exportToYaml(QTextStream* stream)
{
	*stream << m_name << ": " << m_value << "\r\n";
}

void InputConditionContainerString::copyValues(const InputConditionContainerString& i)
{
	m_name = i.m_name;
	m_value = i.m_value;
	m_default = i.m_default;
}
