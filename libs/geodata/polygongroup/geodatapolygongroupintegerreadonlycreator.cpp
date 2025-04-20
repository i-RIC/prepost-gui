#include "geodatapolygongroupintegerreadonlycreator.h"

GeoDataPolygonGroupIntegerReadOnlyCreator::GeoDataPolygonGroupIntegerReadOnlyCreator() :
	GeoDataPolygonGroupIntegerCreator {"integerPolygonGroupReadOnly"}
{}

bool GeoDataPolygonGroupIntegerReadOnlyCreator::isCreatable() const
{
	return false;
}

bool GeoDataPolygonGroupIntegerReadOnlyCreator::isReadOnly() const
{
	return true;
}
