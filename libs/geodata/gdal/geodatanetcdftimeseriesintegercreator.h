#ifndef GEODATANETCDFTIMESERIESINTEGERCREATOR_H
#define GEODATANETCDFTIMESERIESINTEGERCREATOR_H

#include "gd_netcdf_global.h"
#include "geodatanetcdftimeseriescreatort.h"

class vtkIntArray;

class GD_NETCDF_EXPORT GeoDataNetcdfTimeSeriesIntegerCreator : public GeoDataNetcdfTimeSeriesCreatorT<int, vtkIntArray>
{
public:
	GeoDataNetcdfTimeSeriesIntegerCreator();

	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
};

#endif // GEODATANETCDFTIMESERIESINTEGERCREATOR_H
