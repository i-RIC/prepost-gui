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
	iRIC::getDoubleAttribute(node, attName(m_name), m_defaultValue);
}

void DoubleContainer::save(QXmlStreamWriter& writer) const
{
	iRIC::setDoubleAttribute(writer, attName(m_name), m_value);
}
