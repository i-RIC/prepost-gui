#ifndef GEODATAGDALTIMESERIESREALREADONLYCREATOR_H
#define GEODATAGDALTIMESERIESREALREADONLYCREATOR_H

#include "gd_gdal_global.h"
#include "geodatagdaltimeseriesrealcreator.h"

class GD_GDAL_EXPORT GeoDataGdalTimeSeriesRealReadOnlyCreator : public GeoDataGdalTimeSeriesRealCreator
{
public:
	GeoDataGdalTimeSeriesRealReadOnlyCreator();

	bool isReadOnly() const override;
};

#endif // GEODATAGDALTIMESERIESREALREADONLYCREATOR_H
