#include "geodatanetcdfgdalrealimporter.h"
#include "geodatanetcdfimportert.h"
#include "geodatanetcdfrealcreator.h"
#include "geodatanetcdfxbandimporter.h"

GeoDataNetcdfRealCreator::GeoDataNetcdfRealCreator() :
	GeoDataNetcdfCreatorT<double, vtkDoubleArray> {"realNetcdf"}
{
	importers().append(new GeoDataNetcdfRealImporter(this));
	importers().append(new GeoDataNetcdfXbandImporter(this));
	importers().append(new GeoDataNetcdfGdalRealImporter(this));
}

GeoData* GeoDataNetcdfRealCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	GeoDataNetcdf* data = new GeoDataNetcdfReal(parent, this, condition);
	data->setPosition(condition->position());
	data->setDefaultMapper();

	return data;
}
