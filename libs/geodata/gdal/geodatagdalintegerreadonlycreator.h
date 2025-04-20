#ifndef GEODATAGDALINTEGERREADONLYCREATOR_H
#define GEODATAGDALINTEGERREADONLYCREATOR_H

#include "gd_gdal_global.h"
#include "geodatagdalintegercreator.h"

class GD_GDAL_EXPORT GeoDataGdalIntegerReadOnlyCreator : public GeoDataGdalIntegerCreator
{
public:
	GeoDataGdalIntegerReadOnlyCreator();

	bool isReadOnly() const override;
};

#endif // GEODATAGDALINTEGERREADONLYCREATOR_H
