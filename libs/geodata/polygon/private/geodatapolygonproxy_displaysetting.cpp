#include "geodatapolygonproxy_displaysetting.h"

GeoDataPolygonProxy::DisplaySetting::DisplaySetting() :
	CompositeContainer {&usePreSetting, &displaySetting},
	usePreSetting {"usePreSetting", false},
	displaySetting {}
{}

GeoDataPolygonProxy::DisplaySetting::DisplaySetting(const DisplaySetting& s) :
	DisplaySetting {}
{
	copyValue(s);
}

GeoDataPolygonProxy::DisplaySetting& GeoDataPolygonProxy::DisplaySetting::operator=(const DisplaySetting& s)
{
	copyValue(s);
	return *this;
}

XmlAttributeContainer& GeoDataPolygonProxy::DisplaySetting::operator=(const XmlAttributeContainer& s)
{
	return operator=(dynamic_cast<const DisplaySetting&> (s));
}

