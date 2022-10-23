#ifndef MEASUREDDATAVECTORSETTING_H
#define MEASUREDDATAVECTORSETTING_H

#include <guicore/arrows/arrowssettingcontainer.h>
#include <guicore/filter/generalfilteringsettingcontainer.h>
#include <misc/compositecontainer.h>

class MeasuredDataVectorSetting : public CompositeContainer
{
public:
	MeasuredDataVectorSetting();
	MeasuredDataVectorSetting(const MeasuredDataVectorSetting& setting);
	~MeasuredDataVectorSetting();

	MeasuredDataVectorSetting& operator=(const MeasuredDataVectorSetting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	GeneralFilteringSettingContainer filteringSetting;
	ArrowsSettingContainer arrowsSetting;
};

#endif // MEASUREDDATAVECTORSETTING_H
