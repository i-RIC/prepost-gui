#include "geodatapolylinegroup_displaysetting.h"

GeoDataPolyLineGroup::DisplaySetting::DisplaySetting() :
	CompositeContainer ({&color, &opacity, &mapping, &lineWidth}),
	color {"color", Qt::black},
	opacity {},
	mapping {"mapping", Mapping::Value},
	lineWidth {"lineWidth", 1}
{
	opacity = 50;
}

GeoDataPolyLineGroup::DisplaySetting::DisplaySetting(const DisplaySetting& s) :
	DisplaySetting {}
{
	copyValue(s);
}

GeoDataPolyLineGroup::DisplaySetting& GeoDataPolyLineGroup::DisplaySetting::operator=(const DisplaySetting& s)
{
	copyValue(s);
	return *this;
}

XmlAttributeContainer& GeoDataPolyLineGroup::DisplaySetting::operator=(const XmlAttributeContainer& s)
{
	return operator=(dynamic_cast<const DisplaySetting&> (s));
}
