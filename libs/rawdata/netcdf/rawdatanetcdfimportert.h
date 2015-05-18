#ifndef RAWDATANETCDFIMPORTERT_H
#define RAWDATANETCDFIMPORTERT_H

#include "rawdatanetcdft.h"
#include "rawdatanetcdfimporter.h"

#include <guicore/pre/base/preprocessorrawdatagroupdataiteminterface.h>
#include <guicore/pre/gridcond/base/gridrelatedconditiondimensionscontainer.h>
#include <guicore/pre/gridcond/base/gridrelatedconditiondimensioncontainer.h>
#include <misc/stringtool.h>

#include <vtkIntArray.h>
#include <vtkDoubleArray.h>

#include <QList>

template <class V, class DA>
class RawDataNetcdfImporterT : public RawDataNetcdfImporter
{
public:
	RawDataNetcdfImporterT(RawDataCreator* creator) : RawDataNetcdfImporter(creator) {}

protected:
	int importValues(int ncid_in, int icid_out, int varIdOut, int xDimId, int yDimId, int lonDimId, int latDimId, const QList<int> dimIds, RawDataNetcdf* dat) {
		size_t* start_in = new size_t[dimIds.size() + 2];
		size_t* start_out = new size_t[dimIds.size() + 2];
		size_t* len_in = new size_t[dimIds.size() + 2];
		size_t* len_out = new size_t[dimIds.size() + 2];
		int* dimMap = new int [dimIds.size()];
		int xIdx = -1;
		int yIdx = -1;
		int ret;
		int varIdIn;
		V missingValue;

		ret = nc_inq_varid(ncid_in, iRIC::toStr(m_valueVariable).c_str(), &varIdIn);
		ret = ncGetMissingValue(ncid_in, varIdIn, &missingValue);

		int dimnum;
		ret = nc_inq_varndims(ncid_in, varIdIn, &dimnum);
		int* dimids = new int[dimnum];
		ret = nc_inq_vardimid(ncid_in, varIdIn, dimids);
		QList<int> origDimIdList;
		for (int i = 0; i < dimnum; ++i) {
			origDimIdList.append(*(dimids + i));
		}
		delete dimids;

		// setup xIdx, yIdx, dimMap
		for (int i = 0; i < m_dims.size(); ++i) {
			QString dim = m_dims.at(i);
			int dimid;
			ret = nc_inq_dimid(ncid_in, iRIC::toStr(dim).c_str(), &dimid);
			int idx = origDimIdList.indexOf(dimid);
			*(dimMap + i) = idx;
		}
		if (m_csType == RawDataNetcdf::XY) {
			xIdx = origDimIdList.indexOf(xDimId);
			yIdx = origDimIdList.indexOf(yDimId);
		} else if (m_csType == RawDataNetcdf::LonLat) {
			xIdx = origDimIdList.indexOf(lonDimId);
			yIdx = origDimIdList.indexOf(latDimId);
		}

		// setup len_in, len_out
		for (int i = 0; i < dimIds.size(); ++i) {
			*(len_in + *(dimMap + i)) = 1;
			*(len_out + dimIds.size() - 1 - i) = 1;
		}
		size_t bufferSize = 0;
		if (m_csType == RawDataNetcdf::XY) {
			*(len_in + yIdx) = dat->yValues().size();
			*(len_in + xIdx) = dat->xValues().size();
			*(len_out + dimIds.size()) = dat->yValues().size();
			*(len_out + dimIds.size() + 1) = dat->xValues().size();
			bufferSize = dat->xValues().size() * dat->yValues().size();
		} else if (m_csType == RawDataNetcdf::LonLat) {
			*(len_in + yIdx) = dat->latValues().size();
			*(len_in + xIdx) = dat->lonValues().size();
			*(len_out + dimIds.size()) = dat->latValues().size();
			*(len_out + dimIds.size() + 1) = dat->lonValues().size();
			bufferSize = dat->lonValues().size() * dat->latValues().size();
		}

		// setup start_in, start_out partially
		*(start_in + yIdx) = 0;
		*(start_in + xIdx) = 0;
		*(start_out + dimIds.size()) = 0;
		*(start_out + dimIds.size() + 1) = 0;

		V* buffer = new V[bufferSize];

		ret = importSingleLayerValues(ncid_in, icid_out, 0, dimMap, varIdIn, varIdOut, start_in, start_out, len_in, len_out, bufferSize, buffer, missingValue, dat);

		delete buffer;
		delete start_in;
		delete start_out;
		delete len_in;
		delete len_out;
		delete dimMap;

		return ret;
	}
	int importSingleLayerValues(int ncid_in, int ncid_out, int loopid, int* dimMap, int varIdIn, int varIdOut, size_t* start_in, size_t* start_out, size_t* len_in, size_t* len_out, size_t bufferSize, V* buffer, V missingValue, RawDataNetcdf* ncdf) {
		RawDataNetcdfT<V, DA>* netcdf = dynamic_cast<RawDataNetcdfT<V, DA>* >(ncdf);
		GridRelatedConditionDimensionsContainer* dims = m_groupDataItem->dimensions();
		GridRelatedConditionDimensionContainer* c = dims->containers().at(loopid);
		int ret;
		for (int i = 0; i < c->count(); ++i) {
			if (loopid == dims->containers().size() - 1) {
				*(start_in + *(dimMap + loopid)) = i;
				*(start_out + dims->containers().size() - 1 - loopid) = i;
				ret = ncGetVarConvert(ncid_in, varIdIn, start_in, len_in, buffer);
				if (ret != NC_NOERR) {return ret;}
				for (size_t j = 0; j < bufferSize; ++j) {
					if (*(buffer + j) == missingValue) {
						*(buffer + j) = netcdf->missingValue();
					}
				}
				ret = ncPutVarConvert(ncid_out, varIdOut, start_out, len_out, buffer);
				if (ret != NC_NOERR) {return ret;}
			}	else {
				// recursive call
				ret = importSingleLayerValues(ncid_in, ncid_out, loopid + 1, dimMap, varIdIn, varIdOut, start_in, start_out, len_in, len_out, bufferSize, buffer, missingValue, ncdf);
				if (ret != NC_NOERR) { return ret; }
			}
		}
		return NC_NOERR;
	}
	virtual int ncGetVarConvert(int ncid_in, int varId, size_t* start_in, size_t* len_in, V* buffer) = 0;
	virtual int ncPutVarConvert(int ncid_out, int varId, size_t* start_out, size_t* len_out, V* buffer) = 0;
	virtual int ncGetMissingValue(int ncid, int varid, V* value) = 0;
};

class RawDataNetcdfIntegerImporter : public RawDataNetcdfImporterT<int, vtkIntArray>
{
public:
	RawDataNetcdfIntegerImporter(RawDataCreator* creator) : RawDataNetcdfImporterT<int, vtkIntArray>(creator) {}
	int ncGetVarConvert(int ncid_in, int varId, size_t* start_in, size_t* len_in, int* buffer) {
		return nc_get_vara_int(ncid_in, varId, start_in, len_in, buffer);
	}
	int ncPutVarConvert(int ncid_out, int varId, size_t* start_out, size_t* len_out, int* buffer) {
		return nc_put_vara_int(ncid_out, varId, start_out, len_out, buffer);
	}
	int ncGetMissingValue(int ncid, int varid, int* value) {
		int ret;
		ret = nc_get_att_int(ncid, varid, "missing_value", value);
		if (ret == NC_NOERR) {return NC_NOERR;}
		ret = nc_get_att_int(ncid, varid, "_FillValue", value);
		if (ret == NC_NOERR) {return NC_NOERR;}
		*value = NC_FILL_INT;
		return NC_NOERR;
	}
};

class RawDataNetcdfRealImporter : public RawDataNetcdfImporterT<double, vtkDoubleArray>
{
public:
	RawDataNetcdfRealImporter(RawDataCreator* creator) : RawDataNetcdfImporterT<double, vtkDoubleArray>(creator) {}
	int ncGetVarConvert(int ncid_in, int varId, size_t* start_in, size_t* len_in, double* buffer) {
		return nc_get_vara_double(ncid_in, varId, start_in, len_in, buffer);
	}
	int ncPutVarConvert(int ncid_out, int varId, size_t* start_out, size_t* len_out, double* buffer) {
		return nc_put_vara_double(ncid_out, varId, start_out, len_out, buffer);
	}
	int ncGetMissingValue(int ncid, int varid, double* value) {
		int ret;
		ret = nc_get_att_double(ncid, varid, "missing_value", value);
		if (ret == NC_NOERR) {return NC_NOERR;}
		ret = nc_get_att_double(ncid, varid, "_FillValue", value);
		if (ret == NC_NOERR) {return NC_NOERR;}
		*value = NC_FILL_INT;
		return NC_NOERR;
	}
};

#endif // RAWDATANETCDFIMPORTERT_H

