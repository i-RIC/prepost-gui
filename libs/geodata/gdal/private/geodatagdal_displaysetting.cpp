#include "geodatagdal_displaysetting.h"

GeoDataGdal::DisplaySetting::DisplaySetting() :
	CompositeContainer ({&color, &opacity, &mapping}),
	color {"color"},
	opacity {},
	mapping {"mapping", Mapping::Value}
{
	opacity = 50;
}

GeoDataGdal::DisplaySetting::DisplaySetting(const DisplaySetting& s) :
	DisplaySetting {}
{
	copyValue(s);
}

GeoDataGdal::DisplaySetting& GeoDataGdal::DisplaySetting::operator=(const DisplaySetting& s)
{
	copyValue(s);
	return *this;
}

XmlAttributeContainer& GeoDataGdal::DisplaySetting::operator=(const XmlAttributeContainer& s)
{
	return operator=(dynamic_cast<const DisplaySetting&> (s));
}
