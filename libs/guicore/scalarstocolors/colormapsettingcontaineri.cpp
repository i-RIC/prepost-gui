#include "colormapsettingcontaineri.h"
#include "../misc/valuerangecontainer.h"

ColorMapSettingContainerI::ColorMapSettingContainerI() :
	valueCaption {},
	autoMinValue {0},
	autoMaxValue {0}
{}

ColorMapSettingContainerI::~ColorMapSettingContainerI()
{}

void ColorMapSettingContainerI::setAutoValueRange(const ValueRangeContainer& range)
{
	setAutoValueRange(range.minValue, range.maxValue);
}

void ColorMapSettingContainerI::setAutoValueRange(double min, double max)
{
	autoMinValue = min;
	autoMaxValue = max;
}
