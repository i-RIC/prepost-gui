#include "post2dbirdeyewindownodescalargroupdataitem_setting.h"

Post2dBirdEyeWindowNodeScalarGroupDataItem::Setting::Setting() :
	CompositeContainer {&regionSetting, &colorMode, &customColor, &colorTarget, &contourSetting, &opacity},
	regionSetting {},
	colorMode {"colorMode", ColorMode::ByNodeScalar},
	customColor {"customColor", Qt::gray},
	colorTarget {"colorTarget", ""},
	contourSetting {},
	opacity {"opacity", 100}
{}

Post2dBirdEyeWindowNodeScalarGroupDataItem::Setting::Setting(const Setting& setting) :
	Setting {}
{
	copyValue(setting);
}

Post2dBirdEyeWindowNodeScalarGroupDataItem::Setting& Post2dBirdEyeWindowNodeScalarGroupDataItem::Setting::operator=(const Setting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& Post2dBirdEyeWindowNodeScalarGroupDataItem::Setting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Setting&>(c));
}
