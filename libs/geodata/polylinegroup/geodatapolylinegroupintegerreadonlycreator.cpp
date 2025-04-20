#include "geodatapolylinegroupintegerreadonlycreator.h"

GeoDataPolyLineGroupIntegerReadOnlyCreator::GeoDataPolyLineGroupIntegerReadOnlyCreator() :
	GeoDataPolyLineGroupIntegerCreator {"integerPolyLineGroupReadOnly"}
{}

bool GeoDataPolyLineGroupIntegerReadOnlyCreator::isCreatable() const
{
	return false;
}

bool GeoDataPolyLineGroupIntegerReadOnlyCreator::isReadOnly() const
{
	return true;
}
