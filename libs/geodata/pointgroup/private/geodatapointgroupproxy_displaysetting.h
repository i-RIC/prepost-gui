#ifndef GEODATAPOINTGROUPPROXY_DISPLAYSETTING_H
#define GEODATAPOINTGROUPPROXY_DISPLAYSETTING_H

#include "geodatapointgroup_displaysetting.h"
#include "../geodatapointgroupproxy.h"

#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>

class GeoDataPointGroupProxy::DisplaySetting : public CompositeContainer
{
public:
	DisplaySetting();
	DisplaySetting(const DisplaySetting& s);

	DisplaySetting& operator=(const DisplaySetting& s);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& s);

	BoolContainer usePreSetting;
	GeoDataPointGroup::DisplaySetting displaySetting;
};

#endif // GEODATAPOINTGROUPPROXY_DISPLAYSETTING_H
