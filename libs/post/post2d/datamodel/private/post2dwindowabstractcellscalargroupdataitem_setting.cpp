#include "post2dwindowabstractcellscalargroupdataitem_setting.h"

Post2dWindowAbstractCellScalarGroupDataItem::Setting::Setting() :
	CompositeContainer {&regionSetting, &cellSetting},
	colorMapSetting {nullptr},
	regionSetting {},
	cellSetting{}
{}

Post2dWindowAbstractCellScalarGroupDataItem::Setting::Setting(const Setting& setting) :
	Setting {}
{
	copyValue(setting);
}

Post2dWindowAbstractCellScalarGroupDataItem::Setting::~Setting()
{
	delete colorMapSetting;
}

Post2dWindowAbstractCellScalarGroupDataItem::Setting& Post2dWindowAbstractCellScalarGroupDataItem::Setting::operator=(const Setting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& Post2dWindowAbstractCellScalarGroupDataItem::Setting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Setting&>(c));
}

void Post2dWindowAbstractCellScalarGroupDataItem::Setting::load(const QDomNode& node)
{
	CompositeContainer::load(node);
	colorMapSetting->load(node);
}

void Post2dWindowAbstractCellScalarGroupDataItem::Setting::save(QXmlStreamWriter& writer) const
{
	CompositeContainer::save(writer);
	colorMapSetting->save(writer);
}
