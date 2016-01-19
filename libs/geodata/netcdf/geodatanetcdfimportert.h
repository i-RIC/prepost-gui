#ifndef GEODATANETCDFIMPORTERT_H
#define GEODATANETCDFIMPORTERT_H

#include "geodatanetcdft.h"
#include "geodatanetcdfimporter.h"

#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/gridcond/base/gridattributedimensionscontainer.h>
#include <guicore/pre/gridcond/base/gridattributedimensioncontainer.h>
#include <misc/stringtool.h>

#include <vtkIntArray.h>
#include <vtkDoubleArray.h>

#include <QList>

#include <vector>

template <class V, class DA>
class GeoDataNetcdfImporterT : public GeoDataNetcdfImporter
{
public:
	GeoDataNetcdfImporterT(GeoDataCreator* creator);
	virtual ~GeoDataNetcdfImporterT();

private:
	int importValues(int ncid_in, int icid_out, int varIdOut, int xDimId, int yDimId, int lonDimId, int latDimId, const std::vector<int>& dimIds, GeoDataNetcdf* dat) override;
	int importSingleLayerValues(int ncid_in, int ncid_out, int loopid, int* dimMap, int varIdIn, int varIdOut, size_t* start_in, size_t* start_out, size_t* len_in, size_t* len_out, size_t bufferSize, V* buffer, V missingValue, GeoDataNetcdf* ncdf);

	virtual int ncGetVarConvert(int ncid_in, int varId, size_t* start_in, size_t* len_in, V* buffer) const = 0;
	virtual int ncPutVarConvert(int ncid_out, int varId, size_t* start_out, size_t* len_out, V* buffer) const = 0;
	virtual int ncGetMissingValue(int ncid, int varid, V* value) const = 0;
};

#include "private/geodatanetcdfimportert_detail.h"

#endif // GEODATANETCDFIMPORTERT_H
