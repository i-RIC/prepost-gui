#include "geodatagdalreal.h"
#include "geodatagdalgdalrealexporter.h"

#include <gdal.h>
#include <netcdf.h>

#include <vector>

GeoDataGdalGdalRealExporter::GeoDataGdalGdalRealExporter(GeoDataCreator* creator) :
	GeoDataGdalGdalExporter {creator}
{}

GDALDataType GeoDataGdalGdalRealExporter::gdalDataType() const
{
	return GDT_Float64;
}

void GeoDataGdalGdalRealExporter::copyData(GeoDataGdal* gdal, int ncid, int varid, size_t* starts, size_t* ends, GDALRasterBand* band)
{
	size_t buffersize = *(ends) * *(ends + 1);
	std::vector<double> buffer(buffersize);
	std::vector<double> buffer2(buffersize);

	auto gdalr = dynamic_cast<GeoDataGdalReal*> (gdal);

	band->SetNoDataValue(gdalr->missingValue());

	int ret = nc_get_vara_double(ncid, varid, starts, ends, buffer.data());

	for (int j = 0; j < gdal->ySize(); ++j) {
		for (int i = 0; i < gdal->xSize(); ++i) {
			buffer2[(gdal->ySize() - 1 - j) * gdal->xSize() + i] = buffer[j * gdal->xSize() + i];
		}
	}

	GDALRasterIO(band, GF_Write, 0, 0, gdal->xSize(), gdal->ySize(), buffer2.data(), gdal->xSize(), gdal->ySize(), GDT_Float64, 0, 0);
}
