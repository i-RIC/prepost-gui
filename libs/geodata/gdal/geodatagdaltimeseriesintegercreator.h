#ifndef GEODATAGDALTIMESERIESINTEGERCREATOR_H
#define GEODATAGDALTIMESERIESINTEGERCREATOR_H

#include "gd_gdal_global.h"
#include "geodatagdaltimeseriescreatort.h"

class vtkIntArray;

class GD_GDAL_EXPORT GeoDataGdalTimeSeriesIntegerCreator : public GeoDataGdalTimeSeriesCreatorT<int, vtkIntArray>
{
public:
	GeoDataGdalTimeSeriesIntegerCreator();

	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
};

#endif // GEODATAGDALTIMESERIESINTEGERCREATOR_H
