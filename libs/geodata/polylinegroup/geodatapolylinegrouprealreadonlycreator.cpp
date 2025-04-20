#include "geodatapolylinegrouprealreadonlycreator.h"

GeoDataPolyLineGroupRealReadOnlyCreator::GeoDataPolyLineGroupRealReadOnlyCreator() :
	GeoDataPolyLineGroupRealCreator {"realPolyLineGroupReadOnly"}
{}

bool GeoDataPolyLineGroupRealReadOnlyCreator::isCreatable() const
{
	return false;
}

bool GeoDataPolyLineGroupRealReadOnlyCreator::isReadOnly() const
{
	return true;
}
