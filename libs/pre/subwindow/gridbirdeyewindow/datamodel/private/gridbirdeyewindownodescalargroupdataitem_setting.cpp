#include "gridbirdeyewindownodescalargroupdataitem_setting.h"

GridBirdEyeWindowNodeScalarGroupDataItem::Setting::Setting() :
	CompositeContainer {&regionSetting, &colorMode, &customColor, &colorTarget, &opacity},
	regionSetting {},
	colorMode {"colorMode", ColorMode::ByNodeScalar},
	customColor {"customColor", Qt::gray},
	colorTarget {"colorTarget", ""},
	opacity {"opacity", 100}
{}

GridBirdEyeWindowNodeScalarGroupDataItem::Setting::Setting(const Setting& setting) :
	Setting {}
{
	copyValue(setting);
}

GridBirdEyeWindowNodeScalarGroupDataItem::Setting& GridBirdEyeWindowNodeScalarGroupDataItem::Setting::operator=(const Setting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& GridBirdEyeWindowNodeScalarGroupDataItem::Setting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Setting&>(c));
}
