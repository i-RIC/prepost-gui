#include "geodatapointmap_displaysetting.h"

GeoDataPointmap::DisplaySetting::DisplaySetting() :
	CompositeContainer({
										 &tinRepresentation, &tinPointSize,
										 &tinLineWidth, &tinOpacity,
										 &breakLineVisible, &breakLineColor, &breakLineWidth,
										 &polygonsVisible, &polygonsLineWidth, &polygonsOpacity}),
	tinRepresentation {"tinRepresentation", TinRepresentation::Surface},
	tinPointSize {"tinPointSize", 3},
	tinLineWidth {"tinLineWidth", 2},
	tinOpacity {"tinOpacity", 50},

	breakLineVisible {"breakLineVisible", true},
	breakLineColor {"breakLineColor", Qt::black},
	breakLineWidth {"breakLineWidth", 2},

	polygonsVisible {"polygonsVisible", true},
	polygonsLineWidth {"polygonsLineWidth", 1},
	polygonsOpacity {"polygonsOpacity", 50}
{}

GeoDataPointmap::DisplaySetting::DisplaySetting(const DisplaySetting& s) :
	DisplaySetting {}
{
	copyValue(s);
}

GeoDataPointmap::DisplaySetting::~DisplaySetting()
{}

GeoDataPointmap::DisplaySetting& GeoDataPointmap::DisplaySetting::operator=(const DisplaySetting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& GeoDataPointmap::DisplaySetting::operator=(const XmlAttributeContainer& setting)
{
	return operator=(dynamic_cast<const DisplaySetting&> (setting));
}
