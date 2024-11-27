#ifndef GEODATAGDALGDALREALEXPORTER_H
#define GEODATAGDALGDALREALEXPORTER_H

#include "geodatagdalgdalexporter.h"

class GeoDataGdalGdalRealExporter : public GeoDataGdalGdalExporter
{
public:
	GeoDataGdalGdalRealExporter(GeoDataCreator* creator);

private:
	GDALDataType gdalDataType() const override;
	void copyData(GeoDataGdal* gdal, int ncid, int varid, size_t* starts, size_t* ends, GDALRasterBand* band) override;
};

#endif // GEODATAGDALGDALREALEXPORTER_H
