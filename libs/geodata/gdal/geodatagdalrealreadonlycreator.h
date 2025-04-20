#ifndef GEODATAGDALREALREADONLYCREATOR_H
#define GEODATAGDALREALREADONLYCREATOR_H

#include "gd_gdal_global.h"
#include "geodatagdalrealcreator.h"

class GD_GDAL_EXPORT GeoDataGdalRealReadOnlyCreator : public GeoDataGdalRealCreator
{
public:
	GeoDataGdalRealReadOnlyCreator();

	bool isReadOnly() const override;
};

#endif // GEODATAGDALREALREADONLYCREATOR_H
