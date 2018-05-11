#ifndef GEODATANETCDFREALCREATOR_H
#define GEODATANETCDFREALCREATOR_H

#include "gd_netcdf_global.h"
#include "geodatanetcdfcreatort.h"

class vtkDoubleArray;

class GD_NETCDF_EXPORT GeoDataNetcdfRealCreator : public GeoDataNetcdfCreatorT<double, vtkDoubleArray>
{
public:
	GeoDataNetcdfRealCreator();

	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
};

#endif // GEODATANETCDFREALCREATOR_H
