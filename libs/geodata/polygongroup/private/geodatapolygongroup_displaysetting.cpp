#include "geodatapolygongroup_displaysetting.h"

GeoDataPolygonGroup::DisplaySetting::DisplaySetting() :
	CompositeContainer ({&color, &opacity, &mapping, &lineWidth}),
	color {"color", Qt::black},
	opacity {},
	mapping {"mapping", Mapping::Value},
	lineWidth {"lineWidth", 1}
{
	opacity = 50;
}

GeoDataPolygonGroup::DisplaySetting::DisplaySetting(const DisplaySetting& s) :
	DisplaySetting {}
{
	copyValue(s);
}

GeoDataPolygonGroup::DisplaySetting& GeoDataPolygonGroup::DisplaySetting::operator=(const DisplaySetting& s)
{
	copyValue(s);
	return *this;
}

XmlAttributeContainer& GeoDataPolygonGroup::DisplaySetting::operator=(const XmlAttributeContainer& s)
{
	return operator=(dynamic_cast<const DisplaySetting&> (s));
}
