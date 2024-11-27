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
	GeoDataNetcdfTimeSeriesCreatorT<double, vtkDoubleArray> {"timeSeriesRealNetcdf"}
{
	importers().push_back(new GeoDataNetcdfGdalRealImporter(this));
	importers().push_back(new GeoDataNetcdfRealImporter(this));
	importers().push_back(new GeoDataNetcdfXbandImporter(this));

	exporters().push_back(new GeoDataNetcdfNcExporter(this));
}

GeoData* GeoDataNetcdfTimeSeriesRealCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	auto data = new GeoDataNetcdfReal(parent, this, condition);
	if (condition == nullptr || condition->position() == SolverDefinitionGridAttribute::Position::Node) {
		data->setMapper(new GeoDataNetcdfNodeMapperT<double, vtkDoubleArray>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::CellCenter) {
		data->setMapper(new GeoDataNetcdfCellMapperT<double, vtkDoubleArray>(this));
	}
	return data;
}
