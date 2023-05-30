#ifndef MEASUREDDATAPOINTGROUPDATAITEM_SETTING_H
#define MEASUREDDATAPOINTGROUPDATAITEM_SETTING_H

#include "../measureddatapointgroupdataitem.h"

#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>
#include <misc/opacitycontainer.h>
#include <misc/stringcontainer.h>

class MeasuredDataPointGroupDataItem::Setting : public CompositeContainer
{
public:
	enum class ShapeMode {Points, Surface};
	enum class MappingMode {Value, Arbitrary};

	Setting();
	Setting(const Setting& setting);

	Setting& operator=(const Setting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	EnumContainerT<ShapeMode> shapeMode;
	EnumContainerT<MappingMode> mappingMode;
	ColorContainer color;
	StringContainer value;
	OpacityContainer opacity;
	IntContainer pointSize;
};

#endif // MEASUREDDATAPOINTGROUPDATAITEM_SETTING_H
