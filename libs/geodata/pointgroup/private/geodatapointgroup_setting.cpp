#include "geodatapointgroup_setting.h"

GeoDataPointGroup::Setting::Setting() :
	CompositeContainer {&mapping, &shape, &color, &opacity, &pointSize, &imageMaxSize},
	mapping {"mapping", Mapping::Value},
	shape {"shape", Shape::Point},
	color {"color", Qt::black},
	opacity {"opacity", 50},
	pointSize {"pointSize", 3},
	image {},
	imageMaxSize {"imageMaxSize", 64}
{}

GeoDataPointGroup::Setting::Setting(const Setting& s) :
	Setting ()
{
	copyValue(s);
	image = s.image;
}

GeoDataPointGroup::Setting& GeoDataPointGroup::Setting::operator=(const Setting& s)
{
	copyValue(s);
	image = s.image;
	return *this;
}

XmlAttributeContainer& GeoDataPointGroup::Setting::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Setting&> (c));
}
