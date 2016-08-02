#include "measureddatavectorsetting.h"

MeasuredDataVectorSetting::MeasuredDataVectorSetting() :
	ArrowSettingContainer {}
{}

MeasuredDataVectorSetting::MeasuredDataVectorSetting(const MeasuredDataVectorSetting& setting) :
	MeasuredDataVectorSetting {}
{
	copyValue(setting);
}

MeasuredDataVectorSetting& MeasuredDataVectorSetting::operator=(const MeasuredDataVectorSetting& setting)
{
	copyValue(setting);
	return *this;
}
