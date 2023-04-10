#include "geodatanetcdfgdalintegerexporter.h"
#include "geodatanetcdfgdalintegerimporter.h"
#include "geodatanetcdfinteger.h"
#include "geodatanetcdftimeseriesintegercreator.h"
#include "geodatanetcdfintegerimporter.h"
#include "geodatanetcdfncexporter.h"

GeoDataNetcdfTimeSeriesIntegerCreator::GeoDataNetcdfTimeSeriesIntegerCreator() :
	GeoDataNetcdfTimeSeriesCreatorT<int, vtkIntArray> {"integerNetcdf"}
{
	importers().push_back(new GeoDataNetcdfGdalIntegerImporter(this));
	importers().push_back(new GeoDataNetcdfIntegerImporter(this));

	exporters().push_back(new GeoDataNetcdfNcExporter(this));
}

GeoData* GeoDataNetcdfTimeSeriesIntegerCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	auto data = new GeoDataNetcdfInteger(parent, this, condition);
	if (condition == nullptr || condition->position() == SolverDefinitionGridAttribute::Position::Node) {
		data->setMapper(new GeoDataNetcdfNodeMapperT<int, vtkIntArray>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::CellCenter) {
		data->setMapper(new GeoDataNetcdfCellMapperT<int, vtkIntArray>(this));
	}
	return data;
}
