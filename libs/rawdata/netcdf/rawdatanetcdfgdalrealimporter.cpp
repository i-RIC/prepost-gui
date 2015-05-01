#include "rawdatanetcdfgdalrealimporter.h"
#include "rawdatanetcdft.h"
#include <gdal_priv.h>

int RawDataNetcdfGdalRealImporter::outputValues(int ncid, int varId, GDALRasterBand* band, RawDataNetcdf* data)
{
	int ret;
	double* buffer = new double[data->xSize() * data->ySize()];
	double* buffer2 = new double[data->xSize() * data->ySize()];
	band->RasterIO(GF_Read, 0, 0, data->xSize(), data->ySize(), buffer, data->xSize(), data->ySize(), GDT_Float64, 0, 0);
	for (int j = 0; j < data->ySize(); ++j) {
		for (int i = 0; i < data->xSize(); ++i) {
			int srcIndex = i + data->xSize() * (data->ySize() - 1 - j);
			int trgIndex = i + data->xSize() * j;
			*(buffer2 + trgIndex) = *(buffer + srcIndex);
		}
	}
	ret = nc_put_var_double(ncid, varId, buffer2);
	if (ret != NC_NOERR) {return ret;}

	delete buffer2;
	delete buffer;
	return NC_NOERR;
}
