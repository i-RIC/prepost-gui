#include "geodatapolygon_displaysetting.h"

GeoDataPolygon::DisplaySetting::DisplaySetting() :
	CompositeContainer ({&color, &opacity, &mapping, &lineWidth}),
	color {"color", Qt::black},
	opacity {},
	mapping {"mapping", Mapping::Value},
	lineWidth {"lineWidth", 1}
{
	opacity = 50;
}

GeoDataPolygon::DisplaySetting::DisplaySetting(const DisplaySetting& s) :
	DisplaySetting {}
{
	copyValue(s);
}

GeoDataPolygon::DisplaySetting& GeoDataPolygon::DisplaySetting::operator=(const DisplaySetting& s)
{
	copyValue(s);
	return *this;
}

XmlAttributeContainer& GeoDataPolygon::DisplaySetting::operator=(const XmlAttributeContainer& s)
{
	return operator=(dynamic_cast<const DisplaySetting&> (s));
}
