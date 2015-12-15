#include "stringcontainer.h"
#include "stringtool.h"
#include "xmlsupport.h"

#include <QDomNode>
#include <QDomElement>
#include <QXmlStreamWriter>

StringContainer::StringContainer(const QString& name) :
	SimpleValueContainerT<QString> {name}
{}

StringContainer::StringContainer(const QString& name, const QString& defaultVal) :
	SimpleValueContainerT<QString> {name, defaultVal}
{}

StringContainer::StringContainer(const StringContainer& c) :
	SimpleValueContainerT<QString> {c}
{
	m_stdString = iRIC::toStr(m_value);
}

StringContainer::~StringContainer()
{}

void StringContainer::load(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	m_value = elem.attribute(attName(m_name), m_defaultValue);
}

void StringContainer::save(QXmlStreamWriter& writer) const
{
	writer.writeAttribute(attName(m_name), m_value);
}

StringContainer& StringContainer::operator=(const QString& val)
{
	SimpleValueContainerT<QString>::operator =(val);
	m_stdString = iRIC::toStr(val);
	return *this;
}

StringContainer& StringContainer::operator=(const std::string& val)
{
	return operator=(QString(val.c_str()));
}

bool StringContainer::operator!=(const QString& val)
{
	return SimpleValueContainerT<QString>::operator !=(val);
}

StringContainer::operator std::string() const
{
	return m_stdString;
}

StringContainer::operator const char*() const
{
	return m_stdString.c_str();
}

void StringContainer::copyValue(const XmlAttributeContainer& c)
{
	const StringContainer& c2 = dynamic_cast<const StringContainer&> (c);
	m_value = c2.m_value;
}
