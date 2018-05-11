#ifndef GEODATANETCDFIMPORTERT_DETAIL_H
#define GEODATANETCDFIMPORTERT_DETAIL_H

#include "../geodatanetcdfimportert.h"

#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>

template <class V, class DA>
GeoDataNetcdfImporterT<V, DA>::GeoDataNetcdfImporterT(GeoDataCreator* creator) :
	GeoDataNetcdfImporter {creator}
{}

template <class V, class DA>
GeoDataNetcdfImporterT<V, DA>::~GeoDataNetcdfImporterT()
{}

template <class V, class DA>
int GeoDataNetcdfImporterT<V, DA>::importValues(int ncid_in, int icid_out, int varIdOut, int xDimId, int yDimId, int lonDimId, int latDimId, const std::vector<int>& dimIds, GeoDataNetcdf* dat)
{
	std::vector<size_t> start_in(dimIds.size() + 2);
	std::vector<size_t> start_out(dimIds.size() + 2);
	std::vector<size_t> len_in(dimIds.size() + 2);
	std::vector<size_t> len_out(dimIds.size() + 2);
	std::vector<int> dimMap(dimIds.size());

	int xIdx = -1;
	int yIdx = -1;
	int ret;
	int varIdIn;
	V missingValue;
	V scaleFactor;
	V addOffset;

	ret = nc_inq_varid(ncid_in, iRIC::toStr(m_valueVariable).c_str(), &varIdIn);
	ret = ncGetMissingValue(ncid_in, varIdIn, &missingValue);
	ret = ncGetScaleFactorValue(ncid_in, varIdIn, &scaleFactor);
	ret = ncGetAddOffsetValue(ncid_in, varIdIn, &addOffset);

	int dimnum;
	ret = nc_inq_varndims(ncid_in, varIdIn, &dimnum);
	std::vector<int> dimids(dimnum);
	ret = nc_inq_vardimid(ncid_in, varIdIn, dimids.data());
	QList<int> origDimIdList;
	for (int i = 0; i < dimnum; ++i) {
		origDimIdList.append(dimids[i]);
	}

	// setup xIdx, yIdx, dimMap
	for (int i = 0; i < m_dims.size(); ++i) {
		QString dim = m_dims.at(i);
		int dimid;
		ret = nc_inq_dimid(ncid_in, iRIC::toStr(dim).c_str(), &dimid);
		int idx = origDimIdList.indexOf(dimid);
		dimMap[i] = idx;
	}
	if (m_csType == GeoDataNetcdf::XY) {
		xIdx = origDimIdList.indexOf(xDimId);
		yIdx = origDimIdList.indexOf(yDimId);
	} else if (m_csType == GeoDataNetcdf::LonLat) {
		xIdx = origDimIdList.indexOf(lonDimId);
		yIdx = origDimIdList.indexOf(latDimId);
	}

	// setup len_in, len_out
	for (int i = 0; i < dimIds.size(); ++i) {
		len_in[dimMap[i]] = 1;
		len_out[dimIds.size() - 1 - i] = 1;
	}
	size_t bufferSize = 0;
	if (m_csType == GeoDataNetcdf::XY) {
		len_in[yIdx] = dat->yValues().size();
		len_in[xIdx] = dat->xValues().size();
		len_out[dimIds.size()] = dat->yValues().size();
		len_out[dimIds.size() + 1] = dat->xValues().size();
		bufferSize = dat->xValues().size() * dat->yValues().size();
	} else if (m_csType == GeoDataNetcdf::LonLat) {
		len_in[yIdx] = dat->latValues().size();
		len_in[xIdx] = dat->lonValues().size();
		len_out[dimIds.size()] = dat->latValues().size();
		len_out[dimIds.size() + 1] = dat->lonValues().size();
		bufferSize = dat->lonValues().size() * dat->latValues().size();
	}

	// setup start_in, start_out partially
	start_in[yIdx] = 0;
	start_in[xIdx] = 0;
	start_out[dimIds.size()] = 0;
	start_out[dimIds.size() + 1] = 0;

	std::vector<V> buffer(bufferSize);

	ret = importSingleLayerValues(ncid_in, icid_out, 0, dimMap.data(), varIdIn, varIdOut, start_in.data(), start_out.data(), len_in.data(), len_out.data(), bufferSize, buffer.data(), missingValue, scaleFactor, addOffset, dat);

	return ret;
}

template <class V, class DA>
int GeoDataNetcdfImporterT<V, DA>::importSingleLayerValues(int ncid_in, int ncid_out, int loopid, int* dimMap, int varIdIn, int varIdOut, size_t* start_in, size_t* start_out, size_t* len_in, size_t* len_out, size_t bufferSize, V* buffer, V missingValue, V scale, V offset, GeoDataNetcdf* ncdf)
{
	GeoDataNetcdfT<V, DA>* netcdf = dynamic_cast<GeoDataNetcdfT<V, DA>* >(ncdf);
	GridAttributeDimensionsContainer* dims = m_groupDataItem->dimensions();
	GridAttributeDimensionContainer* c = dims->containers().at(loopid);
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
				} else {
					*(buffer + j) = *(buffer + j) * scale + offset;
				}
			}
			ret = ncPutVarConvert(ncid_out, varIdOut, start_out, len_out, buffer);
			if (ret != NC_NOERR) {return ret;}
		}	else {
			// recursive call
			ret = importSingleLayerValues(ncid_in, ncid_out, loopid + 1, dimMap, varIdIn, varIdOut, start_in, start_out, len_in, len_out, bufferSize, buffer, missingValue, scale, offset, ncdf);
			if (ret != NC_NOERR) { return ret; }
		}
	}
	return NC_NOERR;
}

#endif // GEODATANETCDFIMPORTERT_DETAIL_H
