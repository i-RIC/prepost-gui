#ifndef GEODATAPOLYGONGROUPPROXY_DISPLAYSETTING_H
#define GEODATAPOLYGONGROUPPROXY_DISPLAYSETTING_H

#include "geodatapolygongroup_displaysetting.h"
#include "../geodatapolygongroupproxy.h"

#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>

class GeoDataPolygonGroupProxy::DisplaySetting : public CompositeContainer
{
public:
	DisplaySetting();
	DisplaySetting(const DisplaySetting& s);

	DisplaySetting& operator=(const DisplaySetting& s);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& s);

	BoolContainer usePreSetting;
	GeoDataPolygonGroup::DisplaySetting displaySetting;
};

#endif // GEODATAPOLYGONGROUPPROXY_DISPLAYSETTING_H
