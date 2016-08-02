#include "measureddatapointsetting.h"

MeasuredDataPointSetting::MeasuredDataPointSetting() :
	ScalarSettingContainer {},
	pointSize {"pointSize", 3}
{
	addContainer(&pointSize);

	contour = ContourSettingWidget::Points;
}

MeasuredDataPointSetting::MeasuredDataPointSetting(const MeasuredDataPointSetting& setting) :
	MeasuredDataPointSetting {}
{
	copyValue(setting);
}

MeasuredDataPointSetting& MeasuredDataPointSetting::operator=(const MeasuredDataPointSetting& setting)
{
	copyValue(setting);
	return *this;
}
