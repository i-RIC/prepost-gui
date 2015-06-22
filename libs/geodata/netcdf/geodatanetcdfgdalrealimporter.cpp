#include "geodatanetcdfgdalrealimporter.h"
#include "geodatanetcdft.h"

#include <gdal_priv.h>

int GeoDataNetcdfGdalRealImporter::outputValues(int ncid, int varId, GDALRasterBand* band, GeoDataNetcdf* data)
{
	int ret = NC_NOERR;
	int bufSize = data->xSize() * data->ySize();
	std::vector<double> buffer1(bufSize);
	std::vector<double> buffer2(bufSize);
	band->RasterIO(GF_Read, 0, 0, data->xSize(), data->ySize(), buffer1.data(), data->xSize(), data->ySize(), GDT_Float64, 0, 0);
	for (int j = 0; j < data->ySize(); ++j) {
		for (int i = 0; i < data->xSize(); ++i) {
			int srcIndex = i + data->xSize() * (data->ySize() - 1 - j);
			int trgIndex = i + data->xSize() * j;
			buffer2[trgIndex] = buffer1[srcIndex];
		}
	}
	ret = nc_put_var_double(ncid, varId, buffer2.data());
	if (ret != NC_NOERR) {return ret;}
	return NC_NOERR;
}
