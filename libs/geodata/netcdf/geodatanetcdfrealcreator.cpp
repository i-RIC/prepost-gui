#include "geodatanetcdfgdalrealimporter.h"
#include "geodatanetcdfreal.h"
#include "geodatanetcdfrealcreator.h"
#include "geodatanetcdfrealimporter.h"
#include "geodatanetcdfxbandimporter.h"

#include <vtkDoubleArray.h>

GeoDataNetcdfRealCreator::GeoDataNetcdfRealCreator() :
	GeoDataNetcdfCreatorT<double, vtkDoubleArray> {"realNetcdf"}
{
	importers().push_back(new GeoDataNetcdfRealImporter(this));
	importers().push_back(new GeoDataNetcdfXbandImporter(this));
	importers().push_back(new GeoDataNetcdfGdalRealImporter(this));
}

GeoData* GeoDataNetcdfRealCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	GeoDataNetcdf* data = new GeoDataNetcdfReal(parent, this, condition);
	data->setPosition(condition->position());
	data->setDefaultMapper();

	return data;
}
