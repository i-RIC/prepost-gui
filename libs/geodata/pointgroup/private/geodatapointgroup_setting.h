#ifndef GEODATAPOINTGROUP_SETTING_H
#define GEODATAPOINTGROUP_SETTING_H

#include "../geodatapointgroup.h"

#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/intcontainer.h>
#include <misc/enumcontainert.h>
#include <misc/opacitycontainer.h>

class GeoDataPointGroup::Setting : public CompositeContainer
{
public:
	enum class Mapping {Value, Arbitrary};
	enum class Shape {Point, Image};

	Setting();
	Setting(const Setting& s);

	Setting& operator=(const Setting& s);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	EnumContainerT<Mapping> mapping;
	EnumContainerT<Shape> shape;
	ColorContainer color;
	OpacityContainer opacity;
	IntContainer pointSize;
	QImage image;
	IntContainer imageMaxSize;
};

#endif // GEODATAPOINTGROUP_SETTING_H
