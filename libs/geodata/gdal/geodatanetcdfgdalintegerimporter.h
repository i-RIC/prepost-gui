#ifndef GEODATANETCDFGDALINTEGERIMPORTER_H
#define GEODATANETCDFGDALINTEGERIMPORTER_H

#include "gd_netcdf_global.h"
#include "geodatanetcdfgdalimporter.h"

class GD_NETCDF_EXPORT GeoDataNetcdfGdalIntegerImporter : public GeoDataNetcdfGdalImporter
{
public:
	GeoDataNetcdfGdalIntegerImporter(GeoDataCreator* creator);
	~GeoDataNetcdfGdalIntegerImporter();

private:
	int outputValues(int ncid, int varId, GDALRasterBand* band, GeoDataNetcdf* data) override;
	int outputValuesWithTime(int ncid, int varId, int timeId, GDALRasterBand* band, GeoDataNetcdf* data) override;
};

#endif // GEODATANETCDFGDALINTEGERIMPORTER_H
