#include "geodatanetcdfgdalrealimporter.h"
#include "geodatanetcdfreal.h"

#include <vtkDoubleArray.h>

#include <gdal_priv.h>

GeoDataNetcdfGdalRealImporter::GeoDataNetcdfGdalRealImporter(GeoDataCreator* creator) : GeoDataNetcdfGdalImporter(creator)
{}

GeoDataNetcdfGdalRealImporter::~GeoDataNetcdfGdalRealImporter()
{}

int GeoDataNetcdfGdalRealImporter::outputValues(int ncid, int varId, GDALRasterBand* band, GeoDataNetcdf* data)
{
	int ret = NC_NOERR;
	int bufSize = data->xSize() * data->ySize();

	double noDataValue;
	int noDataSuccess;
	noDataValue = band->GetNoDataValue(&noDataSuccess);
	auto rData = dynamic_cast<GeoDataNetcdfReal*> (data);

	std::vector<double> buffer1(bufSize);
	std::vector<double> buffer2(bufSize);
	band->RasterIO(GF_Read, 0, 0, data->xSize(), data->ySize(), buffer1.data(), data->xSize(), data->ySize(), GDT_Float64, 0, 0);
	for (int j = 0; j < data->ySize(); ++j) {
		for (int i = 0; i < data->xSize(); ++i) {
			int srcIndex = i + data->xSize() * (data->ySize() - 1 - j);
			int trgIndex = i + data->xSize() * j;
			double v = buffer1[srcIndex];
			if (noDataSuccess == 1 && v == noDataValue) {
				v = rData->missingValue();
			}
			buffer2[trgIndex] = v;
		}
	}
	ret = nc_put_var_double(ncid, varId, buffer2.data());
	if (ret != NC_NOERR) {return ret;}
	return NC_NOERR;
}

int GeoDataNetcdfGdalRealImporter::outputValuesWithTime(int ncid, int varId, int timeId, GDALRasterBand* band, GeoDataNetcdf* data)
{
	int ret = NC_NOERR;
	int bufSize = data->xSize() * data->ySize();

	double noDataValue;
	int noDataSuccess;
	noDataValue = band->GetNoDataValue(&noDataSuccess);
	auto rData = dynamic_cast<GeoDataNetcdfReal*> (data);

	std::vector<double> buffer1(bufSize);
	std::vector<double> buffer2(bufSize);
	band->RasterIO(GF_Read, 0, 0, data->xSize(), data->ySize(), buffer1.data(), data->xSize(), data->ySize(), GDT_Float64, 0, 0);
	for (int j = 0; j < data->ySize(); ++j) {
		for (int i = 0; i < data->xSize(); ++i) {
			int srcIndex = i + data->xSize() * (data->ySize() - 1 - j);
			int trgIndex = i + data->xSize() * j;
			double v = buffer1[srcIndex];
			if (noDataSuccess == 1 && v == noDataValue) {
				v = rData->missingValue();
			}
			buffer2[trgIndex] = v;
		}
	}
	size_t start[3];
	size_t count[3];

	start[0] = timeId;
	start[1] = 0;
	start[2] = 0;
	count[0] = 1;
	count[1] = data->ySize();
	count[2] = data->xSize();

	ret = nc_put_vara_double(ncid, varId, start, count, buffer2.data());
	if (ret != NC_NOERR) {return ret;}
	return NC_NOERR;
}
