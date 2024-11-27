#ifndef GEODATAGDALGDALINTEGERIMPORTER_H
#define GEODATAGDALGDALINTEGERIMPORTER_H

#include "gd_gdal_global.h"
#include "geodatagdalgdalimporter.h"

class GD_GDAL_EXPORT GeoDataGdalGdalIntegerImporter : public GeoDataGdalGdalImporter
{
public:
	GeoDataGdalGdalIntegerImporter(GeoDataCreator* creator);
	~GeoDataGdalGdalIntegerImporter();

private:
	int outputValues(int ncid, int varId, GDALRasterBand* band, GeoDataGdal* data) override;
	int outputValuesWithTime(int ncid, int varId, int timeId, GDALRasterBand* band, GeoDataGdal* data) override;
};

#endif // GEODATAGDALGDALINTEGERIMPORTER_H
