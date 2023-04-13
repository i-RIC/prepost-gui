#include "geodatapolylineproxy_displaysetting.h"

GeoDataPolyLineProxy::DisplaySetting::DisplaySetting() :
	CompositeContainer {&usePreSetting, &displaySetting},
	usePreSetting {"usePreSetting", true},
	displaySetting {}
{}

GeoDataPolyLineProxy::DisplaySetting::DisplaySetting(const DisplaySetting& s) :
	DisplaySetting {}
{
	copyValue(s);
}

GeoDataPolyLineProxy::DisplaySetting& GeoDataPolyLineProxy::DisplaySetting::operator=(const DisplaySetting& s)
{
	copyValue(s);
	return *this;
}

XmlAttributeContainer& GeoDataPolyLineProxy::DisplaySetting::operator=(const XmlAttributeContainer& s)
{
	return operator=(dynamic_cast<const DisplaySetting&> (s));
}

