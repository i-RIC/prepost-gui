#include "post2dwindownodescalargroupdataitem_setting.h"

Post2dWindowNodeScalarGroupDataItem::Setting::Setting() :
	CompositeContainer {&regionSetting, &opacity, &contourSetting},
	colorMapSetting {nullptr},
	regionSetting {},
	contourSetting {},
	opacity {"opacity", 50}
{}

Post2dWindowNodeScalarGroupDataItem::Setting::Setting(const Setting& setting) :
	Setting {}
{
	copyValue(setting);
}

Post2dWindowNodeScalarGroupDataItem::Setting::~Setting()
{
	delete colorMapSetting;
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

void Post2dWindowNodeScalarGroupDataItem::Setting::load(const QDomNode& node)
{
	CompositeContainer::load(node);
	colorMapSetting->load(node);
}

void Post2dWindowNodeScalarGroupDataItem::Setting::save(QXmlStreamWriter& writer) const
{
	CompositeContainer::save(writer);
	colorMapSetting->save(writer);
}
