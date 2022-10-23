#ifndef VALUERANGECONTAINER_H
#define VALUERANGECONTAINER_H

#include "../guicore_global.h"

#include <misc/compositecontainer.h>
#include <misc/doublecontainer.h>

class GUICOREDLL_EXPORT ValueRangeContainer : public CompositeContainer
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
