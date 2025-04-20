#include "geodatagdaltimeseriesrealreadonlycreator.h"

GeoDataGdalTimeSeriesRealReadOnlyCreator::GeoDataGdalTimeSeriesRealReadOnlyCreator() :
	GeoDataGdalTimeSeriesRealCreator {"timeSeriesRealGdalReadOnly"}
{}

bool GeoDataGdalTimeSeriesRealReadOnlyCreator::isReadOnly() const
{
	return true;
}
