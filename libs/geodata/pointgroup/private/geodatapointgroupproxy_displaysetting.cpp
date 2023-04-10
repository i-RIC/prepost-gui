#include "geodatapointgroupproxy_displaysetting.h"

GeoDataPointGroupProxy::DisplaySetting::DisplaySetting() :
	CompositeContainer {&usePreSetting, &displaySetting},
	usePreSetting {"usePreSetting", true},
	displaySetting {}
{}

GeoDataPointGroupProxy::DisplaySetting::DisplaySetting(const DisplaySetting& s) :
	DisplaySetting {}
{
	copyValue(s);
}

GeoDataPointGroupProxy::DisplaySetting& GeoDataPointGroupProxy::DisplaySetting::operator=(const DisplaySetting& s)
{
	copyValue(s);
	return *this;
}

XmlAttributeContainer& GeoDataPointGroupProxy::DisplaySetting::operator=(const XmlAttributeContainer& s)
{
	return operator=(dynamic_cast<const DisplaySetting&> (s));
}
