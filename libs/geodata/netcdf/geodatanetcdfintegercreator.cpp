#include "geodatanetcdfgdalintegerexporter.h"
#include "geodatanetcdfgdalintegerimporter.h"
#include "geodatanetcdfinteger.h"
#include "geodatanetcdfintegercreator.h"
#include "geodatanetcdfintegerimporter.h"

GeoDataNetcdfIntegerCreator::GeoDataNetcdfIntegerCreator() :
	GeoDataNetcdfCreatorT<int, vtkIntArray> {"integerNetcdf"}
{
	importers().push_back(new GeoDataNetcdfIntegerImporter(this));
	importers().push_back(new GeoDataNetcdfGdalIntegerImporter(this));

	exporters().push_back(new GeoDataNetcdfGdalIntegerExporter(this));
}

GeoData* GeoDataNetcdfIntegerCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	GeoDataNetcdf* data = new GeoDataNetcdfInteger(parent, this, condition);
	data->setPosition(condition->position());
	data->setDefaultMapper();
	return data;
}
