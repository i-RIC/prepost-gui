#include "geodatapolygongrouprealreadonlycreator.h"

GeoDataPolygonGroupRealReadOnlyCreator::GeoDataPolygonGroupRealReadOnlyCreator() :
	GeoDataPolygonGroupRealCreator {"realPolygonGroupReadOnly"}
{}

bool GeoDataPolygonGroupRealReadOnlyCreator::isCreatable() const
{
	return false;
}

bool GeoDataPolygonGroupRealReadOnlyCreator::isReadOnly() const
{
	return true;
}
