#ifndef GEODATAPOINTMAP_TINMANAGER_REMOVETRIANGLESSETTING_H
#define GEODATAPOINTMAP_TINMANAGER_REMOVETRIANGLESSETTING_H

#include "geodatapointmap_tinmanager.h"

#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/doublecontainer.h>

class GeoDataPointmap::TINManager::RemoveTrianglesSetting : public CompositeContainer
{
public:
	RemoveTrianglesSetting();
	RemoveTrianglesSetting(const RemoveTrianglesSetting& s);

	RemoveTrianglesSetting& operator=(const RemoveTrianglesSetting& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	BoolContainer enabled;
	DoubleContainer thresholdLength;
};

#endif // GEODATAPOINTMAP_TINMANAGER_REMOVETRIANGLESSETTING_H
