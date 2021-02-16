#include "geodatanetcdfgdalrealexporter.h"
#include "geodatanetcdfgdalrealimporter.h"
#include "geodatanetcdfncexporter.h"
#include "geodatanetcdfgrayscalepngrealexporter.h"
#include "geodatanetcdfgrayscalepngrealimporter.h"
#include "geodatanetcdfreal.h"
#include "geodatanetcdfrealimporter.h"
#include "geodatanetcdftimeseriesrealcreator.h"
#include "geodatanetcdfxbandimporter.h"

#include <vtkDoubleArray.h>

GeoDataNetcdfTimeSeriesRealCreator::GeoDataNetcdfTimeSeriesRealCreator() :
	GeoDataNetcdfTimeSeriesCreatorT<double, vtkDoubleArray> {"realNetcdf"}
{
	importers().push_back(new GeoDataNetcdfGdalRealImporter(this));
	importers().push_back(new GeoDataNetcdfRealImporter(this));
	importers().push_back(new GeoDataNetcdfXbandImporter(this));

	exporters().push_back(new GeoDataNetcdfNcExporter(this));
}

GeoData* GeoDataNetcdfTimeSeriesRealCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	GeoDataNetcdf* data = new GeoDataNetcdfReal(parent, this, condition);
	data->setPosition(condition->position());
	data->setDefaultMapper();

	return data;
}
