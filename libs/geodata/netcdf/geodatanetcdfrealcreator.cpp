#include "geodatanetcdfgdalrealimporter.h"
#include "geodatanetcdfimportert.h"
#include "geodatanetcdfrealcreator.h"
#include "geodatanetcdfxbandimporter.h"

GeoDataNetcdfRealCreator::GeoDataNetcdfRealCreator()
	: GeoDataNetcdfCreatorT<double, vtkDoubleArray>()
{
	m_typeName = "realNetcdf";
	m_importers.append(new GeoDataNetcdfRealImporter(this));
	m_importers.append(new GeoDataNetcdfXbandImporter(this));
	m_importers.append(new GeoDataNetcdfGdalRealImporter(this));
}

GeoData* GeoDataNetcdfRealCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	GeoDataNetcdf* data = new GeoDataNetcdfReal(parent, this, condition);
	data->setPosition(condition->position());
	data->setDefaultMapper();
	return data;
}
