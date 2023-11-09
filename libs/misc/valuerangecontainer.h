#ifndef VALUERANGECONTAINER_H
#define VALUERANGECONTAINER_H

#include "misc_global.h"

#include "compositecontainer.h"
#include "doublecontainer.h"

class MISCDLL_EXPORT ValueRangeContainer : public CompositeContainer
{
public:
	ValueRangeContainer();
	ValueRangeContainer(const ValueRangeContainer& c);

	ValueRangeContainer& operator=(const ValueRangeContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	void merge(const ValueRangeContainer& range);

	DoubleContainer minValue;
	DoubleContainer maxValue;
};

#endif // VALUERANGECONTAINER_H
