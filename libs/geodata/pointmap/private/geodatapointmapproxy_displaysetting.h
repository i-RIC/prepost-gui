#ifndef GEODATAPOINTMAPPROXY_DISPLAYSETTING_H
#define GEODATAPOINTMAPPROXY_DISPLAYSETTING_H

#include "geodatapointmap_displaysetting.h"
#include "../geodatapointmapproxy.h"

#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>

class GeoDataPointmapProxy::DisplaySetting : public CompositeContainer
{
public:
	DisplaySetting();
	DisplaySetting(const DisplaySetting& s);

	DisplaySetting& operator=(const DisplaySetting& s);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& s);

	BoolContainer usePreSetting;
	GeoDataPointmap::DisplaySetting displaySetting;
};

#endif // GEODATAPOINTMAPPROXY_DISPLAYSETTING_H
