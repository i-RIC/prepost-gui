#include "post2dwindowcellscalargroupdataitem_setting.h"

Post2dWindowCellScalarGroupDataItem::Setting::Setting() :
	CompositeContainer {&regionSetting, &opacity, &colorMapSetting},
	colorMapSetting {},
	regionSetting {},
	opacity {"opacity", 50}
{}

Post2dWindowCellScalarGroupDataItem::Setting::Setting(const Setting& setting) :
	Setting {}
{
	copyValue(setting);
}

Post2dWindowCellScalarGroupDataItem::Setting& Post2dWindowCellScalarGroupDataItem::Setting::operator=(const Setting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& Post2dWindowCellScalarGroupDataItem::Setting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Setting&>(c));
}

