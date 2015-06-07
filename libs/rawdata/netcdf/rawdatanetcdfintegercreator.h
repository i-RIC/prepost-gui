#ifndef RAWDATANETCDFINTEGERCREATOR_H
#define RAWDATANETCDFINTEGERCREATOR_H

#include "rd_netcdf_global.h"
#include "rawdatanetcdfcreatort.h"
#include <vtkIntArray.h>

class RD_NETCDF_EXPORT RawDataNetcdfIntegerCreator : public RawDataNetcdfCreatorT<int, vtkIntArray>
{

public:
	RawDataNetcdfIntegerCreator();
	RawData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
};

#endif // RAWDATANETCDFINTEGERCREATOR_H
