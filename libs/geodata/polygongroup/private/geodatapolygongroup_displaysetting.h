#ifndef GEODATAPOLYGONGROUP_DISPLAYSETTING_H
#define GEODATAPOLYGONGROUP_DISPLAYSETTING_H

#include "../geodatapolygongroup.h"

#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>
#include <misc/opacitycontainer.h>

class GeoDataPolygonGroup::DisplaySetting : public CompositeContainer
{
public:
	enum class Mapping {Value, Arbitrary};

	DisplaySetting();
	DisplaySetting(const DisplaySetting& s);

	DisplaySetting& operator=(const DisplaySetting& s);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& s);

	ColorContainer color;
	OpacityContainer opacity;
	EnumContainerT<Mapping> mapping;
	IntContainer lineWidth;
};

#endif // GEODATAPOLYGONGROUP_DISPLAYSETTING_H
