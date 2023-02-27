#include "polydatasetting.h"

PolyDataSetting::PolyDataSetting() :
	CompositeContainer({&mapping, &color, &value, &opacity, &lineWidth}),
	mapping {"mapping", Mapping::Arbitrary},
	color {"color", Qt::black},
	value {"value", ""},
	opacity {"opacity", 100},
	lineWidth {"lineWidth", 1}
{}

PolyDataSetting::PolyDataSetting(const PolyDataSetting& setting) :
	PolyDataSetting {}
{
	copyValue(setting);
}

PolyDataSetting& PolyDataSetting::operator=(const PolyDataSetting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& PolyDataSetting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const PolyDataSetting&> (c));
}
