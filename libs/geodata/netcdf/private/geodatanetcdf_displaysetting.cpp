#include "geodatanetcdf_displaysetting.h"

GeoDataNetcdf::DisplaySetting::DisplaySetting() :
	CompositeContainer ({&color, &opacity, &mapping}),
	color {"color"},
	opacity {},
	mapping {"mapping", Mapping::Value}
{
	opacity = 50;
}

GeoDataNetcdf::DisplaySetting::DisplaySetting(const DisplaySetting& s) :
	DisplaySetting {}
{
	copyValue(s);
}

GeoDataNetcdf::DisplaySetting& GeoDataNetcdf::DisplaySetting::operator=(const DisplaySetting& s)
{
	copyValue(s);
	return *this;
}

XmlAttributeContainer& GeoDataNetcdf::DisplaySetting::operator=(const XmlAttributeContainer& s)
{
	return operator=(dynamic_cast<const DisplaySetting&> (s));
}
