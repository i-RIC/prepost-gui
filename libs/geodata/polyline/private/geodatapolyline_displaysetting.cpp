#include "geodatapolyline_displaysetting.h"

GeoDataPolyLine::DisplaySetting::DisplaySetting() :
	CompositeContainer ({&color, &opacity, &mapping, &lineWidth}),
	color {"color", Qt::black},
	opacity {},
	mapping {"mapping", Mapping::Value},
	lineWidth {"lineWidth", 1}
{
	opacity = 50;
}

GeoDataPolyLine::DisplaySetting::DisplaySetting(const DisplaySetting& s) :
	DisplaySetting {}
{
	copyValue(s);
}

GeoDataPolyLine::DisplaySetting& GeoDataPolyLine::DisplaySetting::operator=(const DisplaySetting& s)
{
	copyValue(s);
	return *this;
}

XmlAttributeContainer& GeoDataPolyLine::DisplaySetting::operator=(const XmlAttributeContainer& s)
{
	return operator=(dynamic_cast<const DisplaySetting&> (s));
}
