#ifndef GEODATAGDALNETCDFREALIMPORTER_H
#define GEODATAGDALNETCDFREALIMPORTER_H

#include "geodatagdalnetcdfimportert.h"

class GeoDataGdalNetcdfRealImporter : public GeoDataGdalNetcdfImporterT<double, vtkDoubleArray>
{
public:
	GeoDataGdalNetcdfRealImporter(GeoDataCreator* creator);

private:
	int ncGetVarConvert(int ncid_in, int varId, size_t* start_in, size_t* len_in, double* buffer) const override;
	int ncPutVarConvert(int ncid_out, int varId, size_t* start_out, size_t* len_out, double* buffer) const override;
	int ncGetMissingValue(int ncid, int varid, double* value) const override;
	int ncGetScaleFactorValue(int ncid, int varid, double* value) const override;
	int ncGetAddOffsetValue(int ncid, int varid, double* value) const override;
};

#endif // GEODATAGDALNETCDFREALIMPORTER_H
