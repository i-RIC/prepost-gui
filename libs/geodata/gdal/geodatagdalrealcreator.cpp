#include "geodatagdalgdalrealexporter.h"
#include "geodatagdalgdalrealimporter.h"
#include "geodatagdalncexporter.h"
#include "geodatagdalgrayscalepngrealexporter.h"
#include "geodatagdalgrayscalepngrealimporter.h"
#include "geodatagdalifacemappert.h"
#include "geodatagdaljfacemappert.h"
#include "geodatagdalreal.h"
#include "geodatagdalrealcreator.h"
#include "geodatagdalrealimporter.h"
#include "geodatagdalxbandimporter.h"

#include <vtkDoubleArray.h>

GeoDataGdalRealCreator::GeoDataGdalRealCreator() :
	GeoDataGdalCreatorT<double, vtkDoubleArray> {"realGdal"}
{
	importers().push_back(new GeoDataGdalGdalRealImporter(this));
	importers().push_back(new GeoDataGdalRealImporter(this));
	importers().push_back(new GeoDataGdalGrayscalePngRealImporter(this));

	exporters().push_back(new GeoDataGdalGdalRealExporter(this));
	exporters().push_back(new GeoDataGdalGrayscalePngRealExporter(this));
	exporters().push_back(new GeoDataGdalNcExporter(this));
}

GeoData* GeoDataGdalRealCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	GeoDataGdal* data = new GeoDataGdalReal(parent, this, condition);
	if (condition == nullptr || condition->position() == SolverDefinitionGridAttribute::Position::Node) {
		data->setMapper(new GeoDataGdalNodeMapperT<double, vtkDoubleArray>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::CellCenter) {
		data->setMapper(new GeoDataGdalCellMapperT<double, vtkDoubleArray>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::IFace) {
		data->setMapper(new GeoDataGdalIFaceMapperT<double, vtkDoubleArray>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::JFace) {
		data->setMapper(new GeoDataGdalJFaceMapperT<double, vtkDoubleArray>(this));
	}
	return data;
}
