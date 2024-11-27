#ifndef GEODATANETCDFGDALREALIMPORTER_H
#define GEODATANETCDFGDALREALIMPORTER_H

#include "gd_netcdf_global.h"
#include "geodatanetcdfgdalimporter.h"

class GD_NETCDF_EXPORT GeoDataNetcdfGdalRealImporter : public GeoDataNetcdfGdalImporter
{
public:
	GeoDataNetcdfGdalRealImporter(GeoDataCreator* creator);
	~GeoDataNetcdfGdalRealImporter();

private:
	int outputValues(int ncid, int varId, GDALRasterBand* band, GeoDataNetcdf* data) override;
	int outputValuesWithTime(int ncid, int varId, int timeId, GDALRasterBand* band, GeoDataNetcdf* data) override;
};

#endif // GEODATANETCDFGDALREALIMPORTER_H
