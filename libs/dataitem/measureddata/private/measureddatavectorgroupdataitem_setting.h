#ifndef MEASUREDDATAVECTORGROUPDATAITEM_SETTING_H
#define MEASUREDDATAVECTORGROUPDATAITEM_SETTING_H

#include "../measureddatavectorgroupdataitem.h"

#include <guicore/arrows/arrowssettingcontainer.h>
#include <guicore/filter/generalfilteringsettingcontainer.h>
#include <misc/compositecontainer.h>

class MeasuredDataVectorGroupDataItem::Setting : public CompositeContainer
{
public:
	Setting();
	Setting(const Setting& setting);
	~Setting();

	Setting& operator=(const Setting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	GeneralFilteringSettingContainer filteringSetting;
	ArrowsSettingContainer arrowsSetting;
};

#endif // MEASUREDDATAVECTORGROUPDATAITEM_SETTING_H
