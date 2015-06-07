#ifndef RAWDATANETCDFGDALREALIMPORTER_H
#define RAWDATANETCDFGDALREALIMPORTER_H

#include "rd_netcdf_global.h"
#include "rawdatanetcdfgdalimporter.h"

class RD_NETCDF_EXPORT RawDataNetcdfGdalRealImporter : public RawDataNetcdfGdalImporter
{

public:
	RawDataNetcdfGdalRealImporter(RawDataCreator* creator) : RawDataNetcdfGdalImporter(creator) {}
	virtual ~RawDataNetcdfGdalRealImporter() {}

protected:
	int outputValues(int ncid, int varId, GDALRasterBand* band, RawDataNetcdf* data) override;

};

#endif // RAWDATANETCDFGDALREALIMPORTER_H
