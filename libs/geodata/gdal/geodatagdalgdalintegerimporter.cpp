#include "geodatagdalgdalintegerimporter.h"
#include "geodatagdalinteger.h"

#include <vtkIntArray.h>

#include <gdal_priv.h>

GeoDataGdalGdalIntegerImporter::GeoDataGdalGdalIntegerImporter(GeoDataCreator* creator) : GeoDataGdalGdalImporter(creator)
{}

GeoDataGdalGdalIntegerImporter::~GeoDataGdalGdalIntegerImporter()
{}

int GeoDataGdalGdalIntegerImporter::outputValues(int ncid, int varId, int xSize, int ySize, GDALRasterBand* band, GeoDataGdal* data)
{
	int ret = NC_NOERR;

	int noDataValue;
	int noDataSuccess;
	noDataValue = band->GetNoDataValue(&noDataSuccess);
	auto rData = dynamic_cast<GeoDataGdalInteger*> (data);

	std::vector<double> buffer1(xSize, ySize);
	std::vector<int> buffer2(data->xSize() * data->ySize());
	band->RasterIO(GF_Read, 0, 0, xSize, ySize, buffer1.data(), xSize, ySize, GDT_Float64, 0, 0);
	for (int j = 0; j < data->ySize(); ++j) {
		for (int i = 0; i < data->xSize(); ++i) {
			int srcIndex = i + data->xSize() * (data->ySize() - 1 - j);
			int trgIndex = i + data->xSize() * j;

			int srcIndex2 = m_matrix.at(srcIndex);
			int v = 0;
			if (srcIndex2 == -1 || (noDataSuccess == 1 && buffer1[srcIndex2] == noDataValue)) {
				v = rData->missingValue();
			} else {
				v = static_cast<int> (buffer1[srcIndex2]);
			}
			buffer2[trgIndex] = v;
		}
	}
	ret = nc_put_var_int(ncid, varId, buffer2.data());
	if (ret != NC_NOERR) {return ret;}
	return NC_NOERR;
}

int GeoDataGdalGdalIntegerImporter::outputValuesWithTime(int ncid, int varId, int timeId, int xSize, int ySize, GDALRasterBand* band, GeoDataGdal* data)
{
	int ret = NC_NOERR;

	double noDataValue;
	int noDataSuccess;
	noDataValue = band->GetNoDataValue(&noDataSuccess);
	auto rData = dynamic_cast<GeoDataGdalInteger*> (data);

	std::vector<int> buffer1(xSize, ySize);
	std::vector<int> buffer2(data->xSize() * data->ySize());
	band->RasterIO(GF_Read, 0, 0, xSize, ySize, buffer1.data(), xSize, ySize, GDT_Int32, 0, 0);
	for (int j = 0; j < data->ySize(); ++j) {
		for (int i = 0; i < data->xSize(); ++i) {
			int srcIndex = i + data->xSize() * (data->ySize() - 1 - j);
			int trgIndex = i + data->xSize() * j;

			int srcIndex2 = m_matrix.at(srcIndex);
			int v = 0;
			if (srcIndex2 == -1 || (noDataSuccess == 1 && buffer1[srcIndex2] == noDataValue)) {
				v = rData->missingValue();
			} else {
				v = static_cast<int> (buffer1[srcIndex2]);
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
