#include "intcontainer.h"
#include "xmlsupport.h"

IntContainer::IntContainer(const QString& name) :
	SimpleValueContainerT<int> {name}
{}

IntContainer::IntContainer(const QString& name, int defaultVal) :
	SimpleValueContainerT<int> {name, defaultVal}
{}

IntContainer::IntContainer(const IntContainer& c) :
	SimpleValueContainerT<int> {c}
{}

IntContainer::~IntContainer()
{}

void IntContainer::load(const QDomNode& node)
{
	m_value = iRIC::getIntAttribute(node, attName(m_name), m_defaultValue);
}

void IntContainer::save(QXmlStreamWriter& writer) const
{
	iRIC::setIntAttribute(writer, attName(m_name), m_value);
}

IntContainer& IntContainer::operator=(int val)
{
	SimpleValueContainerT<int>::operator =(val);
	return *this;
}

void IntContainer::copyValue(const XmlAttributeContainer& c)
{
	const IntContainer& c2 = dynamic_cast<const IntContainer&> (c);
	m_value = c2.m_value;
}
