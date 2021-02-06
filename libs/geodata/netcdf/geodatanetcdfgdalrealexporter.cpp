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
	std::vector<double> buffer2(buffersize);

	auto netcdfr = dynamic_cast<GeoDataNetcdfReal*> (netcdf);

	band->SetNoDataValue(netcdfr->missingValue());

	int ret = nc_get_vara_double(ncid, varid, starts, ends, buffer.data());

	for (int j = 0; j < netcdf->ySize(); ++j) {
		for (int i = 0; i < netcdf->xSize(); ++i) {
			buffer2[(netcdf->ySize() - 1 - j) * netcdf->xSize() + i] = buffer[j * netcdf->xSize() + i];
		}
	}

	GDALRasterIO(band, GF_Write, 0, 0, netcdf->xSize(), netcdf->ySize(), buffer2.data(), netcdf->xSize(), netcdf->ySize(), GDT_Float64, 0, 0);
}
