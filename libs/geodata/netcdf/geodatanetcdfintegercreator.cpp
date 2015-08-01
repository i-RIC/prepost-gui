#include "geodatanetcdfintegercreator.h"
#include "geodatanetcdft.h"

GeoDataNetcdfIntegerCreator::GeoDataNetcdfIntegerCreator() :
	GeoDataNetcdfCreatorT<int, vtkIntArray> {}
{
	m_typeName = "integerNetcdf";
	m_importers.append(new GeoDataNetcdfIntegerImporter(this));
}

GeoData* GeoDataNetcdfIntegerCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	GeoDataNetcdf* data = new GeoDataNetcdfInteger(parent, this, condition);
	data->setPosition(condition->position());
	data->setDefaultMapper();
	return data;
}
