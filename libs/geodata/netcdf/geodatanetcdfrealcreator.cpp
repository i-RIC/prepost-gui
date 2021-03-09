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
	data->setPosition(condition->position());
	data->setDefaultMapper();

	return data;
}

bool GeoDataNetcdfRealCreator::isCompatibleWith(SolverDefinitionGridAttribute* condition) const
{
	if (condition->isReferenceInformation()) {return false;}
	return true;
}
