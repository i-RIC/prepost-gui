#include "stringcontainer.h"
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
{}

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
	return *this;
}

bool StringContainer::operator!=(const QString& val)
{
	SimpleValueContainerT<QString>::operator !=(val);
	return *this;
}
