#ifndef GEODATAGDALNETCDFIMPORTERT_H
#define GEODATAGDALNETCDFIMPORTERT_H

#include "geodatagdalt.h"
#include "geodatagdalnetcdfimporter.h"

#include <vector>

template <class V, class DA>
class GeoDataGdalNetcdfImporterT : public GeoDataGdalNetcdfImporter
{
public:
	GeoDataGdalNetcdfImporterT(GeoDataCreator* creator);
	virtual ~GeoDataGdalNetcdfImporterT();

private:
	int importValues(int ncid_in, int icid_out, int varIdOut, int xDimId, int yDimId, const std::vector<int>& dimIds, GeoDataGdal* dat) override;
	int importSingleLayerValues(int ncid_in, int ncid_out, int loopid, int* dimMap, int varIdIn, int varIdOut, size_t* start_in, size_t* start_out, size_t* len_in, size_t* len_out, std::vector<V>* srcBuffer, std::vector<V>* tgtBuffer, V missingValue, V scale, V offset, GeoDataGdal* ncdf);
	int importValues(int ncid_in, int ncid_out, int varIdIn, int varIdOut, size_t* start_in, size_t* start_out, size_t* len_in, size_t* len_out, std::vector<V>* srcBuffer, std::vector<V>* tgtBuffer, V missingValue, V newMissingValue, V scale, V offset);

	virtual int ncGetVarConvert(int ncid_in, int varId, size_t* start_in, size_t* len_in, V* buffer) const = 0;
	virtual int ncPutVarConvert(int ncid_out, int varId, size_t* start_out, size_t* len_out, V* buffer) const = 0;
	virtual int ncGetMissingValue(int ncid, int varid, V* value) const = 0;
	virtual int ncGetScaleFactorValue(int ncid, int varid, V* value) const = 0;
	virtual int ncGetAddOffsetValue(int ncid, int varid, V* value) const = 0;
};

#include "private/geodatagdalnetcdfimportert_detail.h"

#endif // GEODATAGDALNETCDFIMPORTERT_H
