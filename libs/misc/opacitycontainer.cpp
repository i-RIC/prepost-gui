#include "opacitycontainer.h"

OpacityContainer::OpacityContainer() :
	OpacityContainer {"opacity", 100}
{}

OpacityContainer::OpacityContainer(const OpacityContainer& c) :
	OpacityContainer {}
{
	copyValue(c);
}

OpacityContainer::OpacityContainer(const QString& name) :
	OpacityContainer {name, 100}
{}

OpacityContainer::OpacityContainer(const QString& name, int defaultVal) :
	CompositeContainer({&enabled, &percent}),
	enabled {"enabled", true},
	percent {"percent", defaultVal}
{
	addPrefix(name);
}

OpacityContainer::~OpacityContainer()
{}

XmlAttributeContainer& OpacityContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const OpacityContainer&> (c));
}

OpacityContainer& OpacityContainer::operator=(const OpacityContainer& c)
{
	copyValue(c);
	emit updated();

	return *this;
}

OpacityContainer& OpacityContainer::operator=(int val)
{
	percent = val;
	emit updated();

	return *this;
}

bool OpacityContainer::operator==(const OpacityContainer& c) const
{
	return (enabled == c.enabled) && (percent == c.percent);
}


int OpacityContainer::opacityPercent() const
{
	if (! enabled) {return 100;}

	return percent;
}

int OpacityContainer::value() const
{
	return percent.value();
}

void OpacityContainer::setValue(int value)
{
	percent.setValue(value);
}

OpacityContainer::operator double() const
{
	if (! enabled) {return 1;}

	return (percent / 100.);
}
