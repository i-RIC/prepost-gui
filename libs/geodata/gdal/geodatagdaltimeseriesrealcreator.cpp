#include "geodatagdalgdalrealexporter.h"
#include "geodatagdalgdalrealimporter.h"
#include "geodatagdalncexporter.h"
#include "geodatagdalgrayscalepngrealexporter.h"
#include "geodatagdalgrayscalepngrealimporter.h"
#include "geodatagdalreal.h"
#include "geodatagdalrealimporter.h"
#include "geodatagdaltimeseriesrealcreator.h"
#include "geodatagdalxbandimporter.h"

#include <vtkDoubleArray.h>

GeoDataGdalTimeSeriesRealCreator::GeoDataGdalTimeSeriesRealCreator() :
	GeoDataGdalTimeSeriesCreatorT<double, vtkDoubleArray> {"timeSeriesRealGdal"}
{
	importers().push_back(new GeoDataGdalGdalRealImporter(this));
	importers().push_back(new GeoDataGdalRealImporter(this));
	importers().push_back(new GeoDataGdalXbandImporter(this));

	exporters().push_back(new GeoDataGdalNcExporter(this));
}

GeoData* GeoDataGdalTimeSeriesRealCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	auto data = new GeoDataGdalReal(parent, this, condition);
	if (condition == nullptr || condition->position() == SolverDefinitionGridAttribute::Position::Node) {
		data->setMapper(new GeoDataGdalNodeMapperT<double, vtkDoubleArray>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::CellCenter) {
		data->setMapper(new GeoDataGdalCellMapperT<double, vtkDoubleArray>(this));
	}
	return data;
}
