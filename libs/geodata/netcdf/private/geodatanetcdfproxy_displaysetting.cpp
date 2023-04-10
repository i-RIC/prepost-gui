#include "geodatanetcdfproxy_displaysetting.h"

GeoDataNetcdfProxy::DisplaySetting::DisplaySetting() :
	CompositeContainer {&usePreSetting, &displaySetting},
	usePreSetting {"usePreSetting", true},
	displaySetting {}
{}

GeoDataNetcdfProxy::DisplaySetting::DisplaySetting(const DisplaySetting& s) :
	DisplaySetting {}
{
	copyValue(s);
}

GeoDataNetcdfProxy::DisplaySetting& GeoDataNetcdfProxy::DisplaySetting::operator=(const DisplaySetting& s)
{
	copyValue(s);
	return *this;
}

XmlAttributeContainer& GeoDataNetcdfProxy::DisplaySetting::operator=(const XmlAttributeContainer& s)
{
	return operator=(dynamic_cast<const DisplaySetting&> (s));
}

