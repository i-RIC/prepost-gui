#include "gridbirdeyewindowcellscalargroupdataitem_setting.h"

GridBirdEyeWindowCellScalarGroupDataItem::Setting::Setting() :
	CompositeContainer {&regionSetting, &colorMode, &customColor, &colorTarget, &opacity},
	regionSetting {},
	colorMode {"colorMode", ColorMode::ByCellScalar},
	customColor {"customColor", Qt::gray},
	colorTarget {"colorTarget", ""},
	opacity {"opacity", 100}
{}

GridBirdEyeWindowCellScalarGroupDataItem::Setting::Setting(const Setting& setting) :
	Setting {}
{
	copyValue(setting);
}

GridBirdEyeWindowCellScalarGroupDataItem::Setting& GridBirdEyeWindowCellScalarGroupDataItem::Setting::operator=(const Setting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& GridBirdEyeWindowCellScalarGroupDataItem::Setting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Setting&>(c));
}
