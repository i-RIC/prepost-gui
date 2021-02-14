#ifndef GEODATANETCDFGDALREALEXPORTER_H
#define GEODATANETCDFGDALREALEXPORTER_H

#include "geodatanetcdfgdalexporter.h"

class GeoDataNetcdfGdalRealExporter : public GeoDataNetcdfGdalExporter
{
public:
	GeoDataNetcdfGdalRealExporter(GeoDataCreator* creator);

private:
	GDALDataType gdalDataType() const override;
	void copyData(GeoDataNetcdf* netcdf, int ncid, int varid, size_t* starts, size_t* ends, GDALRasterBand* band) override;
};

#endif // GEODATANETCDFGDALREALEXPORTER_H
