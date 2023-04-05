#ifndef GEODATAPOLYLINEGROUPPROXY_DISPLAYSETTING_H
#define GEODATAPOLYLINEGROUPPROXY_DISPLAYSETTING_H

#include "geodatapolylinegroup_displaysetting.h"
#include "../geodatapolylinegroupproxy.h"

#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>

class GeoDataPolyLineGroupProxy::DisplaySetting : public CompositeContainer
{
public:
	DisplaySetting();
	DisplaySetting(const DisplaySetting& s);

	DisplaySetting& operator=(const DisplaySetting& s);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& s);

	BoolContainer usePreSetting;
	GeoDataPolyLineGroup::DisplaySetting displaySetting;
};

#endif // GEODATAPOLYLINEGROUPPROXY_DISPLAYSETTING_H
