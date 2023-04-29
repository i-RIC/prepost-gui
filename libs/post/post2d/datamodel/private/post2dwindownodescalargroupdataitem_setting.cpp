#include "post2dwindownodescalargroupdataitem_setting.h"

Post2dWindowNodeScalarGroupDataItem::Setting::Setting() :
	CompositeContainer {&regionSetting, &opacity, &contourSetting, &colorMapSetting},
	colorMapSetting {},
	regionSetting {},
	contourSetting {},
	opacity {"opacity", 50}
{
	contourSetting.setColorMapSetting(&colorMapSetting);
}

Post2dWindowNodeScalarGroupDataItem::Setting::Setting(const Setting& setting) :
	Setting {}
{
	copyValue(setting);
}

Post2dWindowNodeScalarGroupDataItem::Setting& Post2dWindowNodeScalarGroupDataItem::Setting::operator=(const Setting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& Post2dWindowNodeScalarGroupDataItem::Setting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Setting&>(c));
}
