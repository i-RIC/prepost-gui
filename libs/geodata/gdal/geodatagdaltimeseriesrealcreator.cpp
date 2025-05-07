#include "geodatagdalgdalrealexporter.h"
#include "geodatagdalgdalrealimporter.h"
#include "geodatagdalncexporter.h"
#include "geodatagdalgrayscalepngrealexporter.h"
#include "geodatagdalgrayscalepngrealimporter.h"
#include "geodatagdalreal.h"
#include "geodatagdalnetcdfrealimporter.h"
#include "geodatagdaltimeseriesrealcreator.h"
#include "geodatagdalxbandimporter.h"

#include <vtkDoubleArray.h>

GeoDataGdalTimeSeriesRealCreator::GeoDataGdalTimeSeriesRealCreator() :
	GeoDataGdalTimeSeriesRealCreator {"timeSeriesRealGdal"}
{}

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

GeoDataGdalTimeSeriesRealCreator::GeoDataGdalTimeSeriesRealCreator(const QString& typeName) :
	GeoDataGdalTimeSeriesCreatorT<double, vtkDoubleArray> {typeName}
{
	importers().push_back(new GeoDataGdalGdalRealImporter(this));
	importers().push_back(new GeoDataGdalNetcdfRealImporter(this));
	importers().push_back(new GeoDataGdalXbandImporter(this));

	exporters().push_back(new GeoDataGdalNcExporter(this));
}
