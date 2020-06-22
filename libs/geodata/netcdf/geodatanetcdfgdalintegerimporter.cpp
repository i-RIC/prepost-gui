#include "geodatanetcdfgdalintegerimporter.h"
#include "geodatanetcdfinteger.h"

#include <vtkIntArray.h>

#include <gdal_priv.h>

GeoDataNetcdfGdalIntegerImporter::GeoDataNetcdfGdalIntegerImporter(GeoDataCreator* creator) : GeoDataNetcdfGdalImporter(creator)
{}

GeoDataNetcdfGdalIntegerImporter::~GeoDataNetcdfGdalIntegerImporter()
{}

int GeoDataNetcdfGdalIntegerImporter::outputValues(int ncid, int varId, GDALRasterBand* band, GeoDataNetcdf* data)
{
	int ret = NC_NOERR;
	int bufSize = data->xSize() * data->ySize();

	int noDataValue;
	int noDataSuccess;
	noDataValue = band->GetNoDataValue(&noDataSuccess);
	auto rData = dynamic_cast<GeoDataNetcdfInteger*> (data);

	std::vector<double> buffer1(bufSize);
	std::vector<int> buffer2(bufSize);
	band->RasterIO(GF_Read, 0, 0, data->xSize(), data->ySize(), buffer1.data(), data->xSize(), data->ySize(), GDT_Float64, 0, 0);
	for (int j = 0; j < data->ySize(); ++j) {
		for (int i = 0; i < data->xSize(); ++i) {
			int srcIndex = i + data->xSize() * (data->ySize() - 1 - j);
			int trgIndex = i + data->xSize() * j;
			int v;
			if (noDataSuccess == 1 && buffer1[srcIndex] == noDataValue) {
				v = rData->missingValue();
			} else {
				v = static_cast<int> (buffer1[srcIndex]);
			}
			buffer2[trgIndex] = v;
		}
	}
	ret = nc_put_var_int(ncid, varId, buffer2.data());
	if (ret != NC_NOERR) {return ret;}
	return NC_NOERR;
}

int GeoDataNetcdfGdalIntegerImporter::outputValuesWithTime(int ncid, int varId, int timeId, GDALRasterBand* band, GeoDataNetcdf* data)
{
	int ret = NC_NOERR;
	int bufSize = data->xSize() * data->ySize();

	double noDataValue;
	int noDataSuccess;
	noDataValue = band->GetNoDataValue(&noDataSuccess);
	auto rData = dynamic_cast<GeoDataNetcdfInteger*> (data);

	std::vector<double> buffer1(bufSize);
	std::vector<int> buffer2(bufSize);
	band->RasterIO(GF_Read, 0, 0, data->xSize(), data->ySize(), buffer1.data(), data->xSize(), data->ySize(), GDT_Float64, 0, 0);
	for (int j = 0; j < data->ySize(); ++j) {
		for (int i = 0; i < data->xSize(); ++i) {
			int srcIndex = i + data->xSize() * (data->ySize() - 1 - j);
			int trgIndex = i + data->xSize() * j;
			int v;
			if (noDataSuccess == 1 && buffer1[srcIndex] == noDataValue) {
				v = rData->missingValue();
			} else {
				v = static_cast<int> (buffer1[srcIndex]);
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

	ret = nc_put_vara_int(ncid, varId, start, count, buffer2.data());
	if (ret != NC_NOERR) {return ret;}
	return NC_NOERR;
}
