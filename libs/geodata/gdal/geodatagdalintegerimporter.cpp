#include "geodatagdalintegerimporter.h"

GeoDataGdalIntegerImporter::GeoDataGdalIntegerImporter(GeoDataCreator* creator) :
	GeoDataGdalImporterT<int, vtkIntArray>(creator)
{}

int GeoDataGdalIntegerImporter::ncGetVarConvert(int ncid_in, int varId, size_t* start_in, size_t* len_in, int* buffer) const
{
	return nc_get_vara_int(ncid_in, varId, start_in, len_in, buffer);
}

int GeoDataGdalIntegerImporter::ncPutVarConvert(int ncid_out, int varId, size_t* start_out, size_t* len_out, int* buffer) const
{
	return nc_put_vara_int(ncid_out, varId, start_out, len_out, buffer);
}

int GeoDataGdalIntegerImporter::ncGetMissingValue(int ncid, int varid, int* value) const
{
	int ret;
	ret = nc_get_att_int(ncid, varid, "missing_value", value);
	if (ret == NC_NOERR) {return NC_NOERR;}
	ret = nc_get_att_int(ncid, varid, "_FillValue", value);
	if (ret == NC_NOERR) {return NC_NOERR;}
	*value = NC_FILL_INT;
	return NC_NOERR;
}

int GeoDataGdalIntegerImporter::ncGetScaleFactorValue(int ncid, int varid, int* value) const
{
	int ret;
	ret = nc_get_att_int(ncid, varid, "scale_factor", value);
	if (ret == NC_NOERR) {return NC_NOERR;}

	*value = 1;
	return NC_NOERR;
}

int GeoDataGdalIntegerImporter::ncGetAddOffsetValue(int ncid, int varid, int* value) const
{
	int ret;
	ret = nc_get_att_int(ncid, varid, "add_offset", value);
	if (ret == NC_NOERR) {return NC_NOERR;}

	*value = 0;
	return NC_NOERR;
}
