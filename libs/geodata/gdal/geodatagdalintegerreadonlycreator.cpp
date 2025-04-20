#include "geodatagdalintegerreadonlycreator.h"

GeoDataGdalIntegerReadOnlyCreator::GeoDataGdalIntegerReadOnlyCreator() :
	GeoDataGdalIntegerCreator {"integerGdalReadOnly"}
{}

bool GeoDataGdalIntegerReadOnlyCreator::isReadOnly() const
{
	return true;
}
