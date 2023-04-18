#ifndef GEODATAPOLYLINEPROXY_DISPLAYSETTING_H
#define GEODATAPOLYLINEPROXY_DISPLAYSETTING_H

#include "geodatapolyline_displaysetting.h"
#include "../geodatapolylineproxy.h"

#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>

class GeoDataPolyLineProxy::DisplaySetting : public CompositeContainer
{
public:
	DisplaySetting();
	DisplaySetting(const DisplaySetting& s);

	DisplaySetting& operator=(const DisplaySetting& s);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& s);

	BoolContainer usePreSetting;
	GeoDataPolyLine::DisplaySetting displaySetting;
};

#endif // GEODATAPOLYLINEPROXY_DISPLAYSETTING_H
