#include "valuerangecontainer.h"

ValueRangeContainer::ValueRangeContainer() :
	CompositeContainer({&minValue, &maxValue}),
	minValue {"minValue", 0},
	maxValue {"maxValue", 0}
{}

ValueRangeContainer::ValueRangeContainer(const ValueRangeContainer& c) :
	ValueRangeContainer {}
{
	copyValue(c);
}

ValueRangeContainer& ValueRangeContainer::operator=(const ValueRangeContainer& c)
{
	copyValue(c);
	return *this;
}
XmlAttributeContainer& ValueRangeContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const ValueRangeContainer&> (c));
}

void ValueRangeContainer::merge(const ValueRangeContainer& range)
{
	if (range.minValue < minValue) {
		minValue = range.minValue;
	}

	if (range.maxValue > maxValue) {
		maxValue = range.maxValue;
	}
}
