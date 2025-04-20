#include "geodatapointgrouprealreadonlycreator.h"

GeoDataPointGroupRealReadOnlyCreator::GeoDataPointGroupRealReadOnlyCreator() :
	GeoDataPointGroupRealCreator {"realPointGroupReadOnly"}
{}

bool GeoDataPointGroupRealReadOnlyCreator::isCreatable() const
{
	return false;
}

bool GeoDataPointGroupRealReadOnlyCreator::isReadOnly() const
{
	return true;
}
