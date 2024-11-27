#ifndef GEODATAGDALINTEGERCREATOR_H
#define GEODATAGDALINTEGERCREATOR_H

#include "gd_gdal_global.h"
#include "geodatagdalcreatort.h"

class vtkIntArray;

class GD_GDAL_EXPORT GeoDataGdalIntegerCreator : public GeoDataGdalCreatorT<int, vtkIntArray>
{
public:
	GeoDataGdalIntegerCreator();

	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
};

#endif // GEODATAGDALINTEGERCREATOR_H
