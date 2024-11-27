#ifndef GEODATAGDALREALIMPORTER_H
#define GEODATAGDALREALIMPORTER_H

#include "geodatagdalimportert.h"

class GeoDataGdalRealImporter : public GeoDataGdalImporterT<double, vtkDoubleArray>
{
public:
	GeoDataGdalRealImporter(GeoDataCreator* creator);

private:
	int ncGetVarConvert(int ncid_in, int varId, size_t* start_in, size_t* len_in, double* buffer) const override;
	int ncPutVarConvert(int ncid_out, int varId, size_t* start_out, size_t* len_out, double* buffer) const override;
	int ncGetMissingValue(int ncid, int varid, double* value) const override;
	int ncGetScaleFactorValue(int ncid, int varid, double* value) const override;
	int ncGetAddOffsetValue(int ncid, int varid, double* value) const override;
};

#endif // GEODATAGDALREALIMPORTER_H
