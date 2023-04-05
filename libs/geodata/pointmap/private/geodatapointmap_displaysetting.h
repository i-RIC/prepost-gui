#ifndef GEODATAPOINTMAP_DISPLAYSETTING_H
#define GEODATAPOINTMAP_DISPLAYSETTING_H

#include "../geodatapointmap.h"

#include <misc/boolcontainer.h>
#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>
#include <misc/opacitycontainer.h>

class GeoDataPointmap::DisplaySetting : public CompositeContainer
{
public:
	enum class TinRepresentation {
		Points,
		Wireframe,
		Surface,
	};

	DisplaySetting();
	DisplaySetting(const DisplaySetting& s);
	~DisplaySetting();

	DisplaySetting& operator=(const DisplaySetting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& setting);

	EnumContainerT<TinRepresentation> tinRepresentation;
	IntContainer tinPointSize;
	IntContainer tinLineWidth;
	OpacityContainer tinOpacity;

	BoolContainer breakLineVisible;
	ColorContainer breakLineColor;
	IntContainer breakLineWidth;

	BoolContainer polygonsVisible;
	IntContainer polygonsLineWidth;
	OpacityContainer polygonsOpacity;
};

#endif // GEODATAPOINTMAP_DISPLAYSETTING_H
