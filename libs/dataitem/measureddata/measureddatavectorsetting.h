#ifndef MEASUREDDATAVECTORSETTING_H
#define MEASUREDDATAVECTORSETTING_H

#include <misc/arrowsettingcontainer.h>
#include <misc/doublecontainer.h>

class MeasuredDataVectorSetting : public ArrowSettingContainer
{
public:
	MeasuredDataVectorSetting();
	MeasuredDataVectorSetting(const MeasuredDataVectorSetting& setting);

	MeasuredDataVectorSetting& operator=(const MeasuredDataVectorSetting& setting);
};

#endif // MEASUREDDATAVECTORSETTING_H
