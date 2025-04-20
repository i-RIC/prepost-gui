#include "geodatagdaltimeseriesintegerreadonlycreator.h"

GeoDataGdalTimeSeriesIntegerReadOnlyCreator::GeoDataGdalTimeSeriesIntegerReadOnlyCreator() :
	GeoDataGdalTimeSeriesIntegerCreator {"timeSeriesIntegerGdalReadOnly"}
{}

bool GeoDataGdalTimeSeriesIntegerReadOnlyCreator::isReadOnly() const
{
	return true;
}
