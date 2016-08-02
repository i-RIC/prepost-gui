#ifndef MEASUREDDATAPOINTSETTING_H
#define MEASUREDDATAPOINTSETTING_H

#include <guibase/scalarsettingcontainer.h>
#include <misc/intcontainer.h>

class MeasuredDataPointSetting : public ScalarSettingContainer
{
public:
	MeasuredDataPointSetting();
	MeasuredDataPointSetting(const MeasuredDataPointSetting& setting);

	MeasuredDataPointSetting& operator=(const MeasuredDataPointSetting& setting);

	IntContainer pointSize;
};

#endif // MEASUREDDATAPOINTSETTING_H
