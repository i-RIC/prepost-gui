#ifndef GEODATAPOLYGONPROXY_DISPLAYSETTING_H
#define GEODATAPOLYGONPROXY_DISPLAYSETTING_H

#include "geodatapolygon_displaysetting.h"
#include "../geodatapolygonproxy.h"

#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>

class GeoDataPolygonProxy::DisplaySetting : public CompositeContainer
{
public:
	DisplaySetting();
	DisplaySetting(const DisplaySetting& s);

	DisplaySetting& operator=(const DisplaySetting& s);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& s);

	BoolContainer usePreSetting;
	GeoDataPolygon::DisplaySetting displaySetting;
};

#endif // GEODATAPOLYGONPROXY_DISPLAYSETTING_H
