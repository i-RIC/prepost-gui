#ifndef GEODATAGDALGDALINTEGEREXPORTER_H
#define GEODATAGDALGDALINTEGEREXPORTER_H

#include "geodatagdalgdalexporter.h"

class GeoDataGdalGdalIntegerExporter : public GeoDataGdalGdalExporter
{
public:
	GeoDataGdalGdalIntegerExporter(GeoDataCreator* creator);

private:
	GDALDataType gdalDataType() const override;
	void copyData(GeoDataGdal* gdal, int ncid, int varid, size_t* starts, size_t* ends, GDALRasterBand* band) override;
};

#endif // GEODATAGDALGDALINTEGEREXPORTER_H
