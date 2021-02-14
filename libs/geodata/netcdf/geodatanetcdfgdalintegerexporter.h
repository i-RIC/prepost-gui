#ifndef GEODATANETCDFGDALINTEGEREXPORTER_H
#define GEODATANETCDFGDALINTEGEREXPORTER_H

#include "geodatanetcdfgdalexporter.h"

class GeoDataNetcdfGdalIntegerExporter : public GeoDataNetcdfGdalExporter
{
public:
	GeoDataNetcdfGdalIntegerExporter(GeoDataCreator* creator);

private:
	GDALDataType gdalDataType() const override;
	void copyData(GeoDataNetcdf* netcdf, int ncid, int varid, size_t* starts, size_t* ends, GDALRasterBand* band) override;
};

#endif // GEODATANETCDFGDALINTEGEREXPORTER_H
