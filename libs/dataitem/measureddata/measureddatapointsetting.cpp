#include "measureddatapointsetting.h"

#include <QColor>

MeasuredDataPointSetting::MeasuredDataPointSetting() :
	CompositeContainer {&shapeMode, &mappingMode, &color, &value, &opacity, &pointSize},
	shapeMode {"shapeMode", ShapeMode::Points},
	mappingMode {"mappingMode", MappingMode::Arbitrary},
	color {"color", Qt::black},
	value {"value", ""},
	opacity {"opacity", 50},
	pointSize {"pointSize", 3}
{}

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

XmlAttributeContainer& MeasuredDataPointSetting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const MeasuredDataPointSetting&>(c));
}
