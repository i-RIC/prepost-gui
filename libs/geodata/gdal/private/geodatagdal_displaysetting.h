#ifndef GEODATAGDAL_DISPLAYSETTING_H
#define GEODATAGDAL_DISPLAYSETTING_H

#include "../geodatagdal.h"

#include <misc/compositecontainer.h>
#include <misc/opacitycontainer.h>
#include <misc/colorcontainer.h>
#include <misc/enumcontainert.h>

class GeoDataGdal::DisplaySetting : public CompositeContainer
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
};

#endif // GEODATAGDAL_DISPLAYSETTING_H
