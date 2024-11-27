#ifndef GEODATANETCDF_DISPLAYSETTING_H
#define GEODATANETCDF_DISPLAYSETTING_H

#include "../geodatanetcdf.h"

#include <misc/compositecontainer.h>
#include <misc/opacitycontainer.h>
#include <misc/colorcontainer.h>
#include <misc/enumcontainert.h>

class GeoDataNetcdf::DisplaySetting : public CompositeContainer
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

#endif // GEODATANETCDF_DISPLAYSETTING_H
