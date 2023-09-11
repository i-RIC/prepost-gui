#include "post3dwindowcontourgroupdataitem_setting.h"

#include <guicore/scalarstocolors/colormapsettingcontaineri.h>

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

void Post3dWindowContourGroupDataItem::Setting::load(const QDomNode& node)
{
	CompositeContainer::load(node);
	colorMapSetting->load(node);
}

void Post3dWindowContourGroupDataItem::Setting::save(QXmlStreamWriter& writer) const
{
	CompositeContainer::save(writer);
	colorMapSetting->save(writer);
}
