#include "geodatapointgroupintegerreadonlycreator.h"

GeoDataPointGroupIntegerReadOnlyCreator::GeoDataPointGroupIntegerReadOnlyCreator() :
	GeoDataPointGroupIntegerCreator {"integerPointGroupReadOnly"}
{}

bool GeoDataPointGroupIntegerReadOnlyCreator::isCreatable() const
{
	return false;
}

bool GeoDataPointGroupIntegerReadOnlyCreator::isReadOnly() const
{
	return true;
}
