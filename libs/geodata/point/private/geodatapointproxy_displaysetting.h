#ifndef GEODATAPOINTPROXY_DISPLAYSETTING_H
#define GEODATAPOINTPROXY_DISPLAYSETTING_H

#include "geodatapoint_displaysetting.h"
#include "../geodatapointproxy.h"

#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>

class GeoDataPointProxy::DisplaySetting : public CompositeContainer
{
public:
	DisplaySetting();
	DisplaySetting(const DisplaySetting& s);

	DisplaySetting& operator=(const DisplaySetting& s);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& s);

	BoolContainer usePreSetting;
	GeoDataPoint::DisplaySetting displaySetting;
};

#endif // GEODATAPOINTPROXY_DISPLAYSETTING_H
