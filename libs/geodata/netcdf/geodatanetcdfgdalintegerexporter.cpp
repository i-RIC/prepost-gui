#include "geodatanetcdfinteger.h"
#include "geodatanetcdfgdalintegerexporter.h"

GeoDataNetcdfGdalIntegerExporter::GeoDataNetcdfGdalIntegerExporter(GeoDataCreator* creator) :
	GeoDataNetcdfGdalExporter(creator)
{}

GDALDataType GeoDataNetcdfGdalIntegerExporter::gdalDataType() const
{
	return GDT_Int32;
}

void GeoDataNetcdfGdalIntegerExporter::copyData(GeoDataNetcdf* netcdf, int ncid, int varid, size_t* starts, size_t* ends, GDALRasterBand* band)
{
	size_t buffersize = *(ends) * *(ends + 1);
	std::vector<int> buffer(buffersize);
	std::vector<int> buffer2(buffersize);

	auto netcdfi = dynamic_cast<GeoDataNetcdfInteger*> (netcdf);

	band->SetNoDataValue(netcdfi->missingValue());

	int ret = nc_get_vara_int(ncid, varid, starts, ends, buffer.data());

	for (int j = 0; j < netcdf->ySize(); ++j) {
		for (int i = 0; i < netcdf->xSize(); ++i) {
			buffer2[(netcdf->ySize() - 1 - j) * netcdf->xSize() + i] = buffer[j * netcdf->xSize() + i];
		}
	}

	GDALRasterIO(band, GF_Write, 0, 0, netcdf->xSize(), netcdf->ySize(), buffer.data(), netcdf->xSize(), netcdf->ySize(), GDT_Int32, 0, 0);
}
