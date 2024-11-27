#ifndef GEODATANETCDFTIMESERIESREALCREATOR_H
#define GEODATANETCDFTIMESERIESREALCREATOR_H

#include "gd_netcdf_global.h"
#include "geodatanetcdftimeseriescreatort.h"

class vtkDoubleArray;

class GD_NETCDF_EXPORT GeoDataNetcdfTimeSeriesRealCreator : public GeoDataNetcdfTimeSeriesCreatorT<double, vtkDoubleArray>
{
public:
	GeoDataNetcdfTimeSeriesRealCreator();

	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
};

#endif // GEODATANETCDFTIMESERIESREALCREATOR_H
