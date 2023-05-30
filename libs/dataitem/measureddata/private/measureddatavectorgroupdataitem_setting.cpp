#include "measureddatavectorgroupdataitem_setting.h"

MeasuredDataVectorGroupDataItem::Setting::Setting() :
	CompositeContainer({&filteringSetting, &arrowsSetting}),
	filteringSetting {},
	arrowsSetting {}
{}

MeasuredDataVectorGroupDataItem::Setting::Setting(const Setting& setting) :
	Setting {}
{
	copyValue(setting);
}

MeasuredDataVectorGroupDataItem::Setting::~Setting()
{}

MeasuredDataVectorGroupDataItem::Setting& MeasuredDataVectorGroupDataItem::Setting::operator=(const Setting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& MeasuredDataVectorGroupDataItem::Setting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Setting&> (c));
}
