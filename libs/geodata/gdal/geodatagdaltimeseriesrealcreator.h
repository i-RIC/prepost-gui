#ifndef GEODATAGDALTIMESERIESREALCREATOR_H
#define GEODATAGDALTIMESERIESREALCREATOR_H

#include "gd_gdal_global.h"
#include "geodatagdaltimeseriescreatort.h"

class vtkDoubleArray;

class GD_GDAL_EXPORT GeoDataGdalTimeSeriesRealCreator : public GeoDataGdalTimeSeriesCreatorT<double, vtkDoubleArray>
{
public:
	GeoDataGdalTimeSeriesRealCreator();

	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
};

#endif // GEODATAGDALTIMESERIESREALCREATOR_H
