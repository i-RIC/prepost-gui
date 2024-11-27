#include "geodatagdalinteger.h"
#include "geodatagdalgdalintegerexporter.h"

GeoDataGdalGdalIntegerExporter::GeoDataGdalGdalIntegerExporter(GeoDataCreator* creator) :
	GeoDataGdalGdalExporter(creator)
{}

GDALDataType GeoDataGdalGdalIntegerExporter::gdalDataType() const
{
	return GDT_Int32;
}

void GeoDataGdalGdalIntegerExporter::copyData(GeoDataGdal* gdal, int ncid, int varid, size_t* starts, size_t* ends, GDALRasterBand* band)
{
	size_t buffersize = *(ends) * *(ends + 1);
	std::vector<int> buffer(buffersize);
	std::vector<int> buffer2(buffersize);

	auto gdali = dynamic_cast<GeoDataGdalInteger*> (gdal);

	band->SetNoDataValue(gdali->missingValue());

	int ret = nc_get_vara_int(ncid, varid, starts, ends, buffer.data());

	for (int j = 0; j < gdal->ySize(); ++j) {
		for (int i = 0; i < gdal->xSize(); ++i) {
			buffer2[(gdal->ySize() - 1 - j) * gdal->xSize() + i] = buffer[j * gdal->xSize() + i];
		}
	}

	GDALRasterIO(band, GF_Write, 0, 0, gdal->xSize(), gdal->ySize(), buffer2.data(), gdal->xSize(), gdal->ySize(), GDT_Int32, 0, 0);
}
