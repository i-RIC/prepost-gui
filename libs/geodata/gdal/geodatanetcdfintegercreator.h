#ifndef GEODATANETCDFINTEGERCREATOR_H
#define GEODATANETCDFINTEGERCREATOR_H

#include "gd_netcdf_global.h"
#include "geodatanetcdfcreatort.h"

class vtkIntArray;

class GD_NETCDF_EXPORT GeoDataNetcdfIntegerCreator : public GeoDataNetcdfCreatorT<int, vtkIntArray>
{
public:
	GeoDataNetcdfIntegerCreator();

	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
};

#endif // GEODATANETCDFINTEGERCREATOR_H
