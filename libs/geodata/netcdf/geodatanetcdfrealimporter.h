#ifndef GEODATANETCDFREALIMPORTER_H
#define GEODATANETCDFREALIMPORTER_H

#include "geodatanetcdfimportert.h"

class GeoDataNetcdfRealImporter : public GeoDataNetcdfImporterT<double, vtkDoubleArray>
{
public:
	GeoDataNetcdfRealImporter(GeoDataCreator* creator);

private:
	int ncGetVarConvert(int ncid_in, int varId, size_t* start_in, size_t* len_in, double* buffer) const override;
	int ncPutVarConvert(int ncid_out, int varId, size_t* start_out, size_t* len_out, double* buffer) const override;
	int ncGetMissingValue(int ncid, int varid, double* value) const override;
};

#endif // GEODATANETCDFREALIMPORTER_H
