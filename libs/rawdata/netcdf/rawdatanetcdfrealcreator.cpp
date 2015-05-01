#include "rawdatanetcdfrealcreator.h"
#include "rawdatanetcdfimportert.h"
#include "rawdatanetcdfxbandimporter.h"
#include "rawdatanetcdfgdalrealimporter.h"

RawDataNetcdfRealCreator::RawDataNetcdfRealCreator()
	: RawDataNetcdfCreatorT<double, vtkDoubleArray>()
{
	m_typeName = "realNetcdf";
	m_importers.append(new RawDataNetcdfRealImporter(this));
	m_importers.append(new RawDataNetcdfXbandImporter(this));
	m_importers.append(new RawDataNetcdfGdalRealImporter(this));
}

RawData* RawDataNetcdfRealCreator::create(ProjectDataItem* parent, SolverDefinitionGridRelatedCondition* condition)
{
	RawDataNetcdf* data = new RawDataNetcdfReal(parent, this, condition);
	data->setPosition(condition->position());
	data->setDefaultMapper();
	return data;
}
