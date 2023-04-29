#include "gridbirdeyewindowgridshapedataitem_setting.h"

GridBirdEyeWindowGridShapeDataItem::Setting::Setting() :
	CompositeContainer({&elevationTarget, &gridShape}),
	elevationTarget {"elevationTarget"},
	gridShape {}
{}

GridBirdEyeWindowGridShapeDataItem::Setting::Setting(const Setting& setting) :
	Setting {}
{
	copyValue(setting);
}

GridBirdEyeWindowGridShapeDataItem::Setting& GridBirdEyeWindowGridShapeDataItem::Setting::operator=(const Setting& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& GridBirdEyeWindowGridShapeDataItem::Setting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Setting&> (c));
}
