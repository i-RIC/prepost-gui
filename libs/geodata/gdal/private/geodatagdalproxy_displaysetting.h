#ifndef GEODATAGDALPROXY_DISPLAYSETTING_H
#define GEODATAGDALPROXY_DISPLAYSETTING_H

#include "geodatagdal_displaysetting.h"
#include "../geodatagdalproxy.h"

#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>

class GeoDataGdalProxy::DisplaySetting : public CompositeContainer
{
public:
	DisplaySetting();
	DisplaySetting(const DisplaySetting& s);

	DisplaySetting& operator=(const DisplaySetting& s);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& s);

	BoolContainer usePreSetting;
	GeoDataGdal::DisplaySetting displaySetting;
};

#endif // GEODATAGDALPROXY_DISPLAYSETTING_H
