#include "geodatapointproxy_displaysetting.h"

GeoDataPointProxy::DisplaySetting::DisplaySetting() :
	CompositeContainer {&usePreSetting, &displaySetting},
	usePreSetting {"usePreSetting", true},
	displaySetting {}
{}

GeoDataPointProxy::DisplaySetting::DisplaySetting(const DisplaySetting& s) :
	DisplaySetting {}
{
	copyValue(s);
}

GeoDataPointProxy::DisplaySetting& GeoDataPointProxy::DisplaySetting::operator=(const DisplaySetting& s)
{
	copyValue(s);
	return *this;
}

XmlAttributeContainer& GeoDataPointProxy::DisplaySetting::operator=(const XmlAttributeContainer& s)
{
	return operator=(dynamic_cast<const DisplaySetting&> (s));
}
