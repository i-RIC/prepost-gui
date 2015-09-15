#include "doublecontainer.h"
#include "xmlsupport.h"

DoubleContainer::DoubleContainer(const QString& name) :
	SimpleValueContainerT<double> {name}
{}

DoubleContainer::DoubleContainer(const QString& name, double defaultVal) :
	SimpleValueContainerT<double> {name, defaultVal}
{}

DoubleContainer::DoubleContainer(const DoubleContainer& c) :
	SimpleValueContainerT<double> {c}
{}

DoubleContainer::~DoubleContainer()
{}

void DoubleContainer::load(const QDomNode& node)
{
	m_value = iRIC::getDoubleAttribute(node, attName(m_name), m_defaultValue);
}

void DoubleContainer::save(QXmlStreamWriter& writer) const
{
	iRIC::setDoubleAttribute(writer, attName(m_name), m_value);
}

DoubleContainer& DoubleContainer::operator=(double val)
{
	SimpleValueContainerT<double>::operator =(val);
	return *this;
}

DoubleContainer& DoubleContainer::operator-=(double val)
{
	m_value -= val;
	return *this;
}

void DoubleContainer::copyValue(const XmlAttributeContainer& c)
{
	const DoubleContainer& c2 = dynamic_cast<const DoubleContainer&> (c);
	m_value = c2.m_value;
}
