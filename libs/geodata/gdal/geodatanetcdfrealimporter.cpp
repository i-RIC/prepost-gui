#include "geodatanetcdfrealimporter.h"

#include <vtkDoubleArray.h>

GeoDataNetcdfRealImporter::GeoDataNetcdfRealImporter(GeoDataCreator* creator) :
	GeoDataNetcdfImporterT<double, vtkDoubleArray>(creator)
{}

int GeoDataNetcdfRealImporter::ncGetVarConvert(int ncid_in, int varId, size_t* start_in, size_t* len_in, double* buffer) const
{
	return nc_get_vara_double(ncid_in, varId, start_in, len_in, buffer);
}

int GeoDataNetcdfRealImporter::ncPutVarConvert(int ncid_out, int varId, size_t* start_out, size_t* len_out, double* buffer) const
{
	return nc_put_vara_double(ncid_out, varId, start_out, len_out, buffer);
}

int GeoDataNetcdfRealImporter::ncGetMissingValue(int ncid, int varid, double* value) const
{
	int ret;
	ret = nc_get_att_double(ncid, varid, "missing_value", value);
	if (ret == NC_NOERR) {return NC_NOERR;}
	ret = nc_get_att_double(ncid, varid, "_FillValue", value);
	if (ret == NC_NOERR) {return NC_NOERR;}
	*value = NC_FILL_DOUBLE;
	return NC_NOERR;
}

int GeoDataNetcdfRealImporter::ncGetScaleFactorValue(int ncid, int varid, double* value) const
{
	int ret;
	ret = nc_get_att_double(ncid, varid, "scale_factor", value);
	if (ret == NC_NOERR) {return NC_NOERR;}

	*value = 1;
	return NC_NOERR;
}

int GeoDataNetcdfRealImporter::ncGetAddOffsetValue(int ncid, int varid, double* value) const
{
	int ret;
	ret = nc_get_att_double(ncid, varid, "add_offset", value);
	if (ret == NC_NOERR) {return NC_NOERR;}

	*value = 0;
	return NC_NOERR;
}
