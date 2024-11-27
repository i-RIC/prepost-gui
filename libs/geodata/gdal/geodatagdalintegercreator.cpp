#include "geodatagdalgdalintegerexporter.h"
#include "geodatagdalgdalintegerimporter.h"
#include "geodatagdalifacemappert.h"
#include "geodatagdalinteger.h"
#include "geodatagdalintegercreator.h"
#include "geodatagdalintegerimporter.h"
#include "geodatagdaljfacemappert.h"
#include "geodatagdalncexporter.h"

GeoDataGdalIntegerCreator::GeoDataGdalIntegerCreator() :
	GeoDataGdalCreatorT<int, vtkIntArray> {"integerGdal"}
{
	importers().push_back(new GeoDataGdalGdalIntegerImporter(this));
	importers().push_back(new GeoDataGdalIntegerImporter(this));

	exporters().push_back(new GeoDataGdalGdalIntegerExporter(this));
	exporters().push_back(new GeoDataGdalNcExporter(this));
}

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
