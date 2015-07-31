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
	iRIC::getIntAttribute(node, attName(m_name), m_defaultValue);
}

void IntContainer::save(QXmlStreamWriter& writer) const
{
	iRIC::setIntAttribute(writer, attName(m_name), m_value);
}
