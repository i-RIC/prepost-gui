#ifndef GEODATAGDALGDALREALIMPORTER_H
#define GEODATAGDALGDALREALIMPORTER_H

#include "gd_gdal_global.h"
#include "geodatagdalgdalimporter.h"

class GD_GDAL_EXPORT GeoDataGdalGdalRealImporter : public GeoDataGdalGdalImporter
{
public:
	GeoDataGdalGdalRealImporter(GeoDataCreator* creator);
	~GeoDataGdalGdalRealImporter();

private:
	int outputValues(int ncid, int varId, int xSize, int ySize, GDALRasterBand* band, GeoDataGdal* data) override;
	int outputValuesWithTime(int ncid, int varId, int timeId, int xSize, int ySize, GDALRasterBand* band, GeoDataGdal* data) override;
};

#endif // GEODATAGDALGDALREALIMPORTER_H
