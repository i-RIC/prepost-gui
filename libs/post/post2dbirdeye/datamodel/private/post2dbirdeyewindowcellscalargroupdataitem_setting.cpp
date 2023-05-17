#include "post2dbirdeyewindowcellscalargroupdataitem_setting.h"

Post2dBirdEyeWindowCellScalarGroupDataItem::Setting::Setting() :
	CompositeContainer {&regionSetting, &colorMode, &customColor, &colorTarget, &contourSetting, &opacity},
	regionSetting {},
	colorMode {"colorMode", ColorMode::ByCellScalar},
	customColor {"customColor", Qt::gray},
	colorTarget {"colorTarget", ""},
	contourSetting {},
	opacity {"opacity", 100}
{}

Post2dBirdEyeWindowCellScalarGroupDataItem::Setting::Setting(const Setting& setting) :
	Setting {}
{
	copyValue(setting);
}

Post2dBirdEyeWindowCellScalarGroupDataItem::Setting& Post2dBirdEyeWindowCellScalarGroupDataItem::Setting::operator=(const Setting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& Post2dBirdEyeWindowCellScalarGroupDataItem::Setting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Setting&>(c));
}
