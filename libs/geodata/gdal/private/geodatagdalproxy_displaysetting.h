#ifndef GEODATANETCDFPROXY_DISPLAYSETTING_H
#define GEODATANETCDFPROXY_DISPLAYSETTING_H

#include "geodatanetcdf_displaysetting.h"
#include "../geodatanetcdfproxy.h"

#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>

class GeoDataNetcdfProxy::DisplaySetting : public CompositeContainer
{
public:
	DisplaySetting();
	DisplaySetting(const DisplaySetting& s);

	DisplaySetting& operator=(const DisplaySetting& s);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& s);

	BoolContainer usePreSetting;
	GeoDataNetcdf::DisplaySetting displaySetting;
};

#endif // GEODATANETCDFPROXY_DISPLAYSETTING_H
