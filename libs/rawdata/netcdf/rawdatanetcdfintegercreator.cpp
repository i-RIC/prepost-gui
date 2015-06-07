#include "rawdatanetcdfintegercreator.h"
#include "rawdatanetcdft.h"

RawDataNetcdfIntegerCreator::RawDataNetcdfIntegerCreator()
	: RawDataNetcdfCreatorT<int, vtkIntArray>()
{
	m_typeName = "integerNetcdf";
	m_importers.append(new RawDataNetcdfIntegerImporter(this));
}

RawData* RawDataNetcdfIntegerCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	RawDataNetcdf* data = new RawDataNetcdfInteger(parent, this, condition);
	data->setPosition(condition->position());
	data->setDefaultMapper();
	return data;
}
