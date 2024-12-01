#include "geodatagdalgdalintegerexporter.h"
#include "geodatagdalgdalintegerimporter.h"
#include "geodatagdalinteger.h"
#include "geodatagdaltimeseriesintegercreator.h"
#include "geodatagdalnetcdfintegerimporter.h"
#include "geodatagdalncexporter.h"

GeoDataGdalTimeSeriesIntegerCreator::GeoDataGdalTimeSeriesIntegerCreator() :
	GeoDataGdalTimeSeriesCreatorT<int, vtkIntArray> {"timeSeriesIntegerGdal"}
{
	importers().push_back(new GeoDataGdalGdalIntegerImporter(this));
	importers().push_back(new GeoDataGdalNetcdfIntegerImporter(this));

	exporters().push_back(new GeoDataGdalNcExporter(this));
}

GeoData* GeoDataGdalTimeSeriesIntegerCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	auto data = new GeoDataGdalInteger(parent, this, condition);
	if (condition == nullptr || condition->position() == SolverDefinitionGridAttribute::Position::Node) {
		data->setMapper(new GeoDataGdalNodeMapperT<int, vtkIntArray>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::CellCenter) {
		data->setMapper(new GeoDataGdalCellMapperT<int, vtkIntArray>(this));
	}
	return data;
}
