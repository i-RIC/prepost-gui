#ifndef GEODATAGDALTIMESERIESINTEGERREADONLYCREATOR_H
#define GEODATAGDALTIMESERIESINTEGERREADONLYCREATOR_H

#include "gd_gdal_global.h"
#include "geodatagdaltimeseriesintegercreator.h"

class GD_GDAL_EXPORT GeoDataGdalTimeSeriesIntegerReadOnlyCreator : public GeoDataGdalTimeSeriesIntegerCreator
{
public:
	GeoDataGdalTimeSeriesIntegerReadOnlyCreator();

	bool isReadOnly() const override;
};

#endif // GEODATAGDALTIMESERIESINTEGERREADONLYCREATOR_H
