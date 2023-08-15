#include "measureddatapointgroupdataitem_setting.h"

#include <QColor>

MeasuredDataPointGroupDataItem::Setting::Setting() :
	CompositeContainer {&shapeMode, &mappingMode, &color, &value, &opacity, &pointSize},
	shapeMode {"shapeMode", ShapeMode::Points},
	mappingMode {"mappingMode", MappingMode::Arbitrary},
	color {"color", Qt::black},
	value {"value", ""},
	opacity {"opacity", 50},
	pointSize {"pointSize", 3}
{}

MeasuredDataPointGroupDataItem::Setting::Setting(const Setting& setting) :
	Setting {}
{
	copyValue(setting);
}

MeasuredDataPointGroupDataItem::Setting& MeasuredDataPointGroupDataItem::Setting::operator=(const Setting& setting)
{
	copyValue(setting);
	emit updated();
	return *this;
}

XmlAttributeContainer& MeasuredDataPointGroupDataItem::Setting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Setting&>(c));
}
