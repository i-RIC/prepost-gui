#ifndef MEASUREDDATAPOINTSETTING_H
#define MEASUREDDATAPOINTSETTING_H

#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>
#include <misc/opacitycontainer.h>
#include <misc/stringcontainer.h>

class MeasuredDataPointSetting : public CompositeContainer
{
public:
	enum class ShapeMode {Points, Surface};
	enum class MappingMode {Value, Arbitrary};

	MeasuredDataPointSetting();
	MeasuredDataPointSetting(const MeasuredDataPointSetting& setting);

	MeasuredDataPointSetting& operator=(const MeasuredDataPointSetting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	EnumContainerT<ShapeMode> shapeMode;
	EnumContainerT<MappingMode> mappingMode;
	ColorContainer color;
	StringContainer value;
	OpacityContainer opacity;
	IntContainer pointSize;
};

#endif // MEASUREDDATAPOINTSETTING_H
