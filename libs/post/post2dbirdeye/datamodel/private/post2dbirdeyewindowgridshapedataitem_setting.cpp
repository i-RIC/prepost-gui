#include "post2dbirdeyewindowgridshapedataitem_setting.h"

Post2dBirdEyeWindowGridShapeDataItem::Setting::Setting() :
	CompositeContainer({&elevationTarget, &gridShape}),
	elevationTarget {"elevationTarget"},
	gridShape {}
{}

Post2dBirdEyeWindowGridShapeDataItem::Setting::Setting(const Setting& setting) :
	Setting {}
{
	copyValue(setting);
}

Post2dBirdEyeWindowGridShapeDataItem::Setting& Post2dBirdEyeWindowGridShapeDataItem::Setting::operator=(const Setting& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& Post2dBirdEyeWindowGridShapeDataItem::Setting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Setting&> (c));
}
