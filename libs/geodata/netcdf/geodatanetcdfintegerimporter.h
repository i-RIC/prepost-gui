#ifndef GEODATANETCDFINTEGERIMPORTER_H
#define GEODATANETCDFINTEGERIMPORTER_H

#include "geodatanetcdfimportert.h"

class GeoDataNetcdfIntegerImporter : public GeoDataNetcdfImporterT<int, vtkIntArray>
{
public:
	GeoDataNetcdfIntegerImporter(GeoDataCreator* creator);

private:
	int ncGetVarConvert(int ncid_in, int varId, size_t* start_in, size_t* len_in, int* buffer) const override;
	int ncPutVarConvert(int ncid_out, int varId, size_t* start_out, size_t* len_out, int* buffer) const override;
	int ncGetMissingValue(int ncid, int varid, int* value) const override;
};

#endif // GEODATANETCDFINTEGERIMPORTER_H
