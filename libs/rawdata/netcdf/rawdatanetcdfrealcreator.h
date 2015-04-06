#ifndef RAWDATANETCDFREALCREATOR_H
#define RAWDATANETCDFREALCREATOR_H

#include "rd_netcdf_global.h"
#include "rawdatanetcdfcreatort.h"
#include <vtkDoubleArray.h>

class RD_NETCDF_EXPORT RawDataNetcdfRealCreator : public RawDataNetcdfCreatorT<double, vtkDoubleArray>
{
public:
	RawDataNetcdfRealCreator();
	RawData* create(ProjectDataItem* parent, SolverDefinitionGridRelatedCondition *condition);
};

#endif // RAWDATANETCDFREALCREATOR_H
