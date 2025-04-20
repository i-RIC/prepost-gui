#include "geodatagdalcellmappert.h"
#include "geodatagdalgdalintegerexporter.h"
#include "geodatagdalgdalintegerimporter.h"
#include "geodatagdalifacemappert.h"
#include "geodatagdalinteger.h"
#include "geodatagdalintegercreator.h"
#include "geodatagdalnetcdfintegerimporter.h"
#include "geodatagdalnodemappert.h"
#include "geodatagdaljfacemappert.h"
#include "geodatagdalncexporter.h"

GeoDataGdalIntegerCreator::GeoDataGdalIntegerCreator() :
	GeoDataGdalIntegerCreator {"integerGdal"}
{}

GeoData* GeoDataGdalIntegerCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	auto data = new GeoDataGdalInteger(parent, this, condition);
	if (condition == nullptr || condition->position() == SolverDefinitionGridAttribute::Position::Node) {
		data->setMapper(new GeoDataGdalNodeMapperT<int, vtkIntArray>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::CellCenter) {
		data->setMapper(new GeoDataGdalCellMapperT<int, vtkIntArray>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::IFace) {
		data->setMapper(new GeoDataGdalIFaceMapperT<int, vtkIntArray>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::JFace) {
		data->setMapper(new GeoDataGdalJFaceMapperT<int, vtkIntArray>(this));
	}
	return data;
}

GeoDataGdalIntegerCreator::GeoDataGdalIntegerCreator(const QString& typeName) :
	GeoDataGdalCreatorT<int, vtkIntArray> {typeName}
{
	importers().push_back(new GeoDataGdalGdalIntegerImporter(this));
	importers().push_back(new GeoDataGdalNetcdfIntegerImporter(this));

	exporters().push_back(new GeoDataGdalGdalIntegerExporter(this));
	exporters().push_back(new GeoDataGdalNcExporter(this));
}
