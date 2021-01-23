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

	auto netcdfi = dynamic_cast<GeoDataNetcdfInteger*> (netcdf);

	band->SetNoDataValue(netcdfi->missingValue());

	int ret = nc_get_vara_int(ncid, varid, starts, ends, buffer.data());
	GDALRasterIO(band, GF_Write, 0, 0, netcdf->xSize(), netcdf->ySize(), buffer.data(), netcdf->xSize(), netcdf->ySize(), GDT_Int32, 0, 0);
}
