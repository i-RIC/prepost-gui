#include "post3dwindownodescalargroupdataitem_setting.h"

Post3dWindowNodeScalarGroupDataItem::Setting::Setting() :
	CompositeContainer {&target, &fullRange, &range, &isoValue, &color, &opacity},
	target {"solution", ""},
	fullRange {"fullRange", true},
	range {},
	isoValue {"value", 0},
	color {"color", Qt::white},
	opacity {}
{}

Post3dWindowNodeScalarGroupDataItem::Setting::Setting(const Setting& setting) :
	Setting {}
{
	copyValue(setting);
}

Post3dWindowNodeScalarGroupDataItem::Setting& Post3dWindowNodeScalarGroupDataItem::Setting::operator=(const Setting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& Post3dWindowNodeScalarGroupDataItem::Setting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Setting&>(c));
}
