#include "geodatagdalrealreadonlycreator.h"

GeoDataGdalRealReadOnlyCreator::GeoDataGdalRealReadOnlyCreator() :
	GeoDataGdalRealCreator {"realGdalReadOnly"}
{}

bool GeoDataGdalRealReadOnlyCreator::isReadOnly() const
{
	return true;
}
