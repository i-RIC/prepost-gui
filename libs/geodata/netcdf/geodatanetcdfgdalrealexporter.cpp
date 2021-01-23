#include "geodatanetcdfreal.h"
#include "geodatanetcdfgdalrealexporter.h"

#include <gdal.h>
#include <netcdf.h>

#include <vector>

GeoDataNetcdfGdalRealExporter::GeoDataNetcdfGdalRealExporter(GeoDataCreator* creator) :
	GeoDataNetcdfGdalExporter {creator}
{}

GDALDataType GeoDataNetcdfGdalRealExporter::gdalDataType() const
{
	return GDT_Float64;
}

void GeoDataNetcdfGdalRealExporter::copyData(GeoDataNetcdf* netcdf, int ncid, int varid, size_t* starts, size_t* ends, GDALRasterBand* band)
{
	size_t buffersize = *(ends) * *(ends + 1);
	std::vector<double> buffer(buffersize);

	auto netcdfr = dynamic_cast<GeoDataNetcdfReal*> (netcdf);

	band->SetNoDataValue(netcdfr->missingValue());

	int ret = nc_get_vara_double(ncid, varid, starts, ends, buffer.data());
	GDALRasterIO(band, GF_Write, 0, 0, netcdf->xSize(), netcdf->ySize(), buffer.data(), netcdf->xSize(), netcdf->ySize(), GDT_Float64, 0, 0);
}
