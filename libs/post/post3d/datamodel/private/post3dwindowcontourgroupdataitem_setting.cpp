#include "post3dwindowcontourgroupdataitem_setting.h"

Post3dWindowContourGroupDataItem::Setting::Setting() :
	CompositeContainer({&contourSetting, &lighting, &opacity}),
	contourSetting {},
	lighting {"lighting", false},
	opacity {"opacity", 100}
{}

Post3dWindowContourGroupDataItem::Setting::Setting(const Setting& s) :
	Setting {}
{
	copyValue(s);
}

Post3dWindowContourGroupDataItem::Setting& Post3dWindowContourGroupDataItem::Setting::operator=(const Setting& s)
{
	copyValue(s);
	return *this;
}

XmlAttributeContainer& Post3dWindowContourGroupDataItem::Setting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Setting&> (c));
}
