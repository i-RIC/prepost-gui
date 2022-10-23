#include "measureddatavectorsetting.h"

MeasuredDataVectorSetting::MeasuredDataVectorSetting() :
	CompositeContainer({&filteringSetting, &arrowsSetting}),
	filteringSetting {},
	arrowsSetting {}
{}

MeasuredDataVectorSetting::MeasuredDataVectorSetting(const MeasuredDataVectorSetting& setting) :
	MeasuredDataVectorSetting {}
{
	copyValue(setting);
}

MeasuredDataVectorSetting::~MeasuredDataVectorSetting()
{}

MeasuredDataVectorSetting& MeasuredDataVectorSetting::operator=(const MeasuredDataVectorSetting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& MeasuredDataVectorSetting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const MeasuredDataVectorSetting&> (c));
}
