#ifndef GEODATAPOINTGROUP_SCALESIZEPAIR_H
#define GEODATAPOINTGROUP_SCALESIZEPAIR_H

#include "../geodatapointgroup.h"

#include <misc/compositecontainer.h>
#include <misc/doublecontainer.h>

class GeoDataPointGroup::ScaleSizePair : public CompositeContainer
{
public:
	ScaleSizePair();
	ScaleSizePair(const ScaleSizePair& pair);
	ScaleSizePair(double scale, double size);

	ScaleSizePair& operator=(const ScaleSizePair& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	DoubleContainer scale;
	DoubleContainer size;
};

#endif // GEODATAPOINTGROUP_SCALESIZEPAIR_H
