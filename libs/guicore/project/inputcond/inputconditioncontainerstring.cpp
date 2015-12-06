#include "inputconditioncontainerstring.h"

#include <misc/stringtool.h>

#include <QDomNode>
#include <QTextStream>

#include <yaml-cpp/yaml.h>

#include <iriclib.h>

InputConditionContainerString::InputConditionContainerString() :
	InputConditionContainer()
{}

InputConditionContainerString::InputConditionContainerString(const QString& n, const QString& c, const QDomNode& defNode) :
	InputConditionContainer(n, c)
{
	setup(defNode);
}

InputConditionContainerString::InputConditionContainerString(const InputConditionContainerString& i) :
	InputConditionContainer(i)
{
	copyValues(i);
}

InputConditionContainerString::~InputConditionContainerString()
{}

InputConditionContainerString& InputConditionContainerString::operator=(const InputConditionContainerString& i)
{
	copyValues(i);
	return *this;
}

const QString& InputConditionContainerString::value() const {
	return m_value;
}

void InputConditionContainerString::setValue(const QString& v)
{
	if (m_value != v) {
		m_value = v;
		emit valueChanged(m_value);
		emit valueChanged();
	}
}

int InputConditionContainerString::load()
{
	char buffer[200];
	int ret;
	if (isBoundaryCondition()) {
		ret = cg_iRIC_Read_BC_String(const_cast<char*>(iRIC::toStr(bcName()).c_str()), bcIndex(), const_cast<char*>(iRIC::toStr(name()).c_str()), buffer);
	} else if (isComplexCondition()) {
		ret = cg_iRIC_Read_Complex_String(const_cast<char*>(iRIC::toStr(complexName()).c_str()), complexIndex(), const_cast<char*>(iRIC::toStr(name()).c_str()), buffer);
	} else {
		ret = cg_iRIC_Read_String(const_cast<char*>(iRIC::toStr(name()).c_str()), buffer);
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
	if (isBoundaryCondition()) {
		return cg_iRIC_Write_BC_String(const_cast<char*>(iRIC::toStr(bcName()).c_str()), bcIndex(), const_cast<char*>(iRIC::toStr(name()).c_str()), const_cast<char*>(value.c_str()));
	} else if (isComplexCondition()) {
		return cg_iRIC_Write_Complex_String(const_cast<char*>(iRIC::toStr(complexName()).c_str()), complexIndex(), const_cast<char*>(iRIC::toStr(name()).c_str()), const_cast<char*>(value.c_str()));
	} else {
		return cg_iRIC_Write_String(const_cast<char*>(iRIC::toStr(name()).c_str()), const_cast<char*>(value.c_str()));
	}
}

void InputConditionContainerString::clear()
{
	m_value = m_default;
	emit valueChanged(m_value);
}

QVariant InputConditionContainerString::variantValue() const
{
	return QVariant(m_value);
}

void InputConditionContainerString::importFromYaml(const YAML::Node& doc, const QDir&)
{
	if (doc[iRIC::toStr(name())]) {
		m_value = doc[iRIC::toStr(name())].as<std::string>().c_str();
		emit valueChanged(m_value);
		emit valueChanged();
	}
}

void InputConditionContainerString::exportToYaml(QTextStream* stream, const QDir&)
{
	*stream << name() << ": " << m_value << "\t#[string] " << caption() << "\r\n";
}

void InputConditionContainerString::setup(const QDomNode& defNode)
{
	QDomElement e = defNode.toElement();
	m_default = e.attribute("default", "");
	m_value = m_default;
}

void InputConditionContainerString::copyValues(const InputConditionContainerString& i)
{
	InputConditionContainer::copyValues(i);
	m_value = i.m_value;
	m_default = i.m_default;
}
