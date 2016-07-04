#include "inputconditioncontainerstring.h"

#include <misc/stringtool.h>

#include <QDomNode>
#include <QTextStream>

#include <yaml-cpp/yaml.h>

#include <iriclib.h>

InputConditionContainerString::InputConditionContainerString() :
	InputConditionContainer()
{}

InputConditionContainerString::InputConditionContainerString(const std::string& n, const QString& c, const QDomNode& defNode) :
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
	char* buffer;
	int ret;
	int length;

	if (isBoundaryCondition()){
		ret = cg_iRIC_Read_BC_StringLen(const_cast<char*>(bcName().c_str()), bcIndex(), const_cast<char*>(name().c_str()), &length);
	} else if (isComplexCondition()){
		ret = cg_iRIC_Read_Complex_StringLen(const_cast<char*>(complexName().c_str()), complexIndex(), const_cast<char*>(name().c_str()), &length);
	} else {
		ret = cg_iRIC_Read_StringLen(const_cast<char*>(name().c_str()), &length);
	}

	if (ret != 0){
		clear();
		return ret;
	}

	buffer = new char[length + 1];

	if (isBoundaryCondition()){
		ret = cg_iRIC_Read_BC_String(const_cast<char*>(bcName().c_str()), bcIndex(), const_cast<char*>(name().c_str()), buffer);
	} else if (isComplexCondition()){
		ret = cg_iRIC_Read_Complex_String(const_cast<char*>(complexName().c_str()), complexIndex(), const_cast<char*>(name().c_str()), buffer);
	} else {
		ret = cg_iRIC_Read_String(const_cast<char*>(name().c_str()), buffer);
	}
	if (ret != 0){
		clear();
	} else {
		m_value = buffer;
		emit valueChanged(m_value);
		emit valueChanged();
	}
	delete buffer;

	return ret;
}

int InputConditionContainerString::save()
{
	std::string value = m_value.toUtf8().constData();
	if (isBoundaryCondition()) {
		return cg_iRIC_Write_BC_String(toC(bcName()), bcIndex(), toC(name()), toC(value));
	} else if (isComplexCondition()) {
		return cg_iRIC_Write_Complex_String(toC(complexName()), complexIndex(), toC(name()), toC(value));
	} else {
		return cg_iRIC_Write_String(toC(name()), toC(value));
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
	if (doc[name()]) {
		m_value = doc[name()].as<std::string>().c_str();
		emit valueChanged(m_value);
		emit valueChanged();
	}
}

void InputConditionContainerString::exportToYaml(QTextStream* stream, const QDir&)
{
	*stream << name().c_str() << ": " << m_value << "\t#[string] " << caption() << "\r\n";
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
