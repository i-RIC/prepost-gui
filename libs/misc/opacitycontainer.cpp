#include "opacitycontainer.h"

OpacityContainer::OpacityContainer() :
	OpacityContainer("opacityPercent")
{}

OpacityContainer::OpacityContainer(const QString& name) :
	IntContainer {name, 100}
{}

OpacityContainer::OpacityContainer(const QString& name, int defaultVal) :
	IntContainer {name, defaultVal}
{}

OpacityContainer::~OpacityContainer()
{}

XmlAttributeContainer& OpacityContainer::operator=(const XmlAttributeContainer& c)
{
	const auto& c2 = dynamic_cast<const OpacityContainer&> (c);
	m_value = c2.m_value;
	emit updated();

	return *this;
}

OpacityContainer& OpacityContainer::operator=(const OpacityContainer& c)
{
	m_value = c.m_value;
	emit updated();

	return *this;
}

OpacityContainer& OpacityContainer::operator=(int val)
{
	m_value = val;
	emit updated();

	return *this;
}

OpacityContainer::operator double() const
{
	return m_value / 100.;
}
