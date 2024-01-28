#ifndef GEODATAPOINTMAP_RITER3DSETTING_H
#define GEODATAPOINTMAP_RITER3DSETTING_H

#include "../geodatapointmap.h"

#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/stringcontainer.h>

class GeoDataPointmap::Riter3dSetting : public CompositeContainer
{
public:
	Riter3dSetting();
	Riter3dSetting(const Riter3dSetting& s);

	Riter3dSetting& operator=(const Riter3dSetting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& setting);

	BoolContainer enabled;
	StringContainer fileName;
};

#endif // GEODATAPOINTMAP_RITER3DSETTING_H
