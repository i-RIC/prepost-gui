#ifndef GEODATAGDALREALCREATOR_H
#define GEODATAGDALREALCREATOR_H

#include "gd_gdal_global.h"
#include "geodatagdalcreatort.h"

class vtkDoubleArray;

class GD_GDAL_EXPORT GeoDataGdalRealCreator : public GeoDataGdalCreatorT<double, vtkDoubleArray>
{
public:
	GeoDataGdalRealCreator();

	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
};

#endif // GEODATAGDALREALCREATOR_H
