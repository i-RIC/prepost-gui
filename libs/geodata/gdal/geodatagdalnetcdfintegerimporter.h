#ifndef GEODATAGDALNETCDFINTEGERIMPORTER_H
#define GEODATAGDALNETCDFINTEGERIMPORTER_H

#include "geodatagdalnetcdfimportert.h"

class GeoDataGdalNetcdfIntegerImporter : public GeoDataGdalNetcdfImporterT<int, vtkIntArray>
{
public:
	GeoDataGdalNetcdfIntegerImporter(GeoDataCreator* creator);

private:
	int ncGetVarConvert(int ncid_in, int varId, size_t* start_in, size_t* len_in, int* buffer) const override;
	int ncPutVarConvert(int ncid_out, int varId, size_t* start_out, size_t* len_out, int* buffer) const override;
	int ncGetMissingValue(int ncid, int varid, int* value) const override;
	int ncGetScaleFactorValue(int ncid, int varid, int* value) const override;
	int ncGetAddOffsetValue(int ncid, int varid, int* value) const override;
};

#endif // GEODATAGDALNETCDFINTEGERIMPORTER_H
