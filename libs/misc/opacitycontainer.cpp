#include "opacitycontainer.h"

OpacityContainer::OpacityContainer() :
	OpacityContainer("opacity")
{}

OpacityContainer::OpacityContainer(const QString& name) :
	IntContainer {name, 100}
{}

OpacityContainer::~OpacityContainer()
{}

OpacityContainer& OpacityContainer::operator=(const OpacityContainer& c)
{
	m_value = c.m_value;
	return *this;
}

OpacityContainer& OpacityContainer::operator=(int val)
{
	m_value = val;
	return *this;
}

OpacityContainer::operator double() const
{
	return m_value / 100.;
}
