#include "geodatanetcdfgdalrealexporter.h"
#include "geodatanetcdfgdalrealimporter.h"
#include "geodatanetcdfncexporter.h"
#include "geodatanetcdfgrayscalepngrealexporter.h"
#include "geodatanetcdfgrayscalepngrealimporter.h"
#include "geodatanetcdfreal.h"
#include "geodatanetcdfrealcreator.h"
#include "geodatanetcdfrealimporter.h"
#include "geodatanetcdfxbandimporter.h"

#include <vtkDoubleArray.h>

GeoDataNetcdfRealCreator::GeoDataNetcdfRealCreator() :
	GeoDataNetcdfCreatorT<double, vtkDoubleArray> {"realNetcdf"}
{
	importers().push_back(new GeoDataNetcdfGdalRealImporter(this));
	importers().push_back(new GeoDataNetcdfRealImporter(this));
	importers().push_back(new GeoDataNetcdfGrayscalePngRealImporter(this));

	exporters().push_back(new GeoDataNetcdfGdalRealExporter(this));
	exporters().push_back(new GeoDataNetcdfGrayscalePngRealExporter(this));
	exporters().push_back(new GeoDataNetcdfNcExporter(this));
}

GeoData* GeoDataNetcdfRealCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	GeoDataNetcdf* data = new GeoDataNetcdfReal(parent, this, condition);
	if (condition == nullptr || condition->position() == SolverDefinitionGridAttribute::Position::Node) {
		data->setMapper(new GeoDataNetcdfNodeMapperT<double, vtkDoubleArray>(this));
	} else if (condition->position() == SolverDefinitionGridAttribute::Position::CellCenter) {
		data->setMapper(new GeoDataNetcdfCellMapperT<double, vtkDoubleArray>(this));
	}
	return data;
}
