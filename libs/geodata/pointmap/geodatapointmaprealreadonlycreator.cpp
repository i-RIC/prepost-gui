#include "geodatapointmaprealreadonlycreator.h"

GeoDataPointmapRealReadOnlyCreator::GeoDataPointmapRealReadOnlyCreator() :
	GeoDataPointmapRealCreator {"realPointmapReadOnly"}
{}

bool GeoDataPointmapRealReadOnlyCreator::isReadOnly() const
{
	return true;
}
