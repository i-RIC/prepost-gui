#include "geodatapointgroup_scalesizepair.h"

GeoDataPointGroup::ScaleSizePair::ScaleSizePair() :
	ScaleSizePair {0, 0}
{}

GeoDataPointGroup::ScaleSizePair::ScaleSizePair(const ScaleSizePair& pair) :
	ScaleSizePair {}
{
	copyValue(pair);
}

GeoDataPointGroup::ScaleSizePair::ScaleSizePair(double v, double s) :
	CompositeContainer ({&scale, &size}),
	scale {"scale", v},
	size {"size", s}
{}

GeoDataPointGroup::ScaleSizePair& GeoDataPointGroup::ScaleSizePair::operator=(const ScaleSizePair& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& GeoDataPointGroup::ScaleSizePair::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const ScaleSizePair&> (c));
}
