#ifndef GEODATAGDALNETCDFIMPORTERT_DETAIL_H
#define GEODATAGDALNETCDFIMPORTERT_DETAIL_H

#include "../geodatagdalnetcdfimportert.h"

#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>

template <class V, class DA>
GeoDataGdalNetcdfImporterT<V, DA>::GeoDataGdalNetcdfImporterT(GeoDataCreator* creator) :
	GeoDataGdalNetcdfImporter {creator}
{}

template <class V, class DA>
GeoDataGdalNetcdfImporterT<V, DA>::~GeoDataGdalNetcdfImporterT()
{}

template <class V, class DA>
int GeoDataGdalNetcdfImporterT<V, DA>::importValues(int ncid_in, int icid_out, int varIdOut, int xDimId, int yDimId, const std::vector<int>& dimIds, GeoDataGdal* dat)
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
	xIdx = origDimIdList.indexOf(xDimId);
	yIdx = origDimIdList.indexOf(yDimId);

	// setup len_in, len_out
	for (int i = 0; i < dimIds.size(); ++i) {
		len_in[dimMap[i]] = 1;
		len_out[dimIds.size() - 1 - i] = 1;
	}
	len_in[yIdx] = m_srcJSize;
	len_in[xIdx] = m_srcISize;
	len_out[dimIds.size()] = dat->yValues().size();
	len_out[dimIds.size() + 1] = dat->xValues().size();
	auto srcBufferSize = m_srcISize * m_srcJSize;
	auto tgtBufferSize = dat->xValues().size() * dat->yValues().size();

	// setup start_in, start_out partially
	start_in[yIdx] = 0;
	start_in[xIdx] = 0;
	start_out[dimIds.size()] = 0;
	start_out[dimIds.size() + 1] = 0;

	std::vector<V> srcBuffer(srcBufferSize);
	std::vector<V> tgtBuffer(tgtBufferSize);

	ret = importSingleLayerValues(ncid_in, icid_out, 0, dimMap.data(), varIdIn, varIdOut, start_in.data(), start_out.data(), len_in.data(), len_out.data(), &srcBuffer, &tgtBuffer, missingValue, scaleFactor, addOffset, dat);

	return ret;
}

template <class V, class DA>
int GeoDataGdalNetcdfImporterT<V, DA>::importSingleLayerValues(int ncid_in, int ncid_out, int loopid, int* dimMap, int varIdIn, int varIdOut, size_t* start_in, size_t* start_out, size_t* len_in, size_t* len_out, std::vector<V>* srcBuffer, std::vector<V>* tgtBuffer, V missingValue, V scale, V offset, GeoDataGdal* ncdf)
{
	GeoDataGdalT<V, DA>* gdal = dynamic_cast<GeoDataGdalT<V, DA>* >(ncdf);
	GridAttributeDimensionsContainer* dims = m_groupDataItem->dimensions();
	if (dims->containers().size() == 0) {
		*(start_in) = 0;
		*(start_out) = 0;
		return importValues(ncid_in, ncid_out, varIdIn, varIdOut, start_in, start_out, len_in, len_out, srcBuffer, tgtBuffer, missingValue, gdal->missingValue(), scale, offset);
	} else {
		GridAttributeDimensionContainer* c = dims->containers().at(loopid);
		int ret;
		for (int i = 0; i < c->count(); ++i) {
			if (loopid == dims->containers().size() - 1) {
				*(start_in + *(dimMap + loopid)) = i;
				*(start_out + dims->containers().size() - 1 - loopid) = i;
				ret = importValues(ncid_in, ncid_out, varIdIn, varIdOut, start_in, start_out, len_in, len_out, srcBuffer, tgtBuffer, missingValue, gdal->missingValue(), scale, offset);
				if (ret != NC_NOERR) { return ret; }
			} else {
				// recursive call
				ret = importSingleLayerValues(ncid_in, ncid_out, loopid + 1, dimMap, varIdIn, varIdOut, start_in, start_out, len_in, len_out, srcBuffer, tgtBuffer, missingValue, scale, offset, ncdf);
				if (ret != NC_NOERR) { return ret; }
			}
		}
		return NC_NOERR;
	}
}

template <class V, class DA>
int GeoDataGdalNetcdfImporterT<V, DA>::importValues(int ncid_in, int ncid_out, int varIdIn, int varIdOut, size_t* start_in, size_t* start_out, size_t* len_in, size_t* len_out, std::vector<V>* srcBuffer, std::vector<V>* tgtBuffer, V missingValue, V newMissingValue, V scale, V offset)
{
	int ret = ncGetVarConvert(ncid_in, varIdIn, start_in, len_in, srcBuffer->data());
	if (ret != NC_NOERR) { return ret; }

	for (int j = 0; j < m_tgtJSize; ++j) {
		for (int i = 0; i < m_tgtISize; ++i) {
			int srcIndex = i + m_tgtISize * (m_tgtJSize - 1 - j);
			int tgtIndex = i + m_tgtISize * (m_tgtJSize - 1 - j);

			int srcIndex2 = m_matrix.at(srcIndex);
			V v = 0;
			if (srcIndex2 == -1 || (*srcBuffer)[srcIndex2] == missingValue) {
				v = newMissingValue;
			} else {
				v = (*srcBuffer)[srcIndex2];
				v = v * scale + offset;
			}
			(*tgtBuffer)[tgtIndex] = v;
		}
	}

	ret = ncPutVarConvert(ncid_out, varIdOut, start_out, len_out, tgtBuffer->data());
	if (ret != NC_NOERR) { return ret; }
	return NC_NOERR;
}

#endif // GEODATAGDALNETCDFIMPORTERT_DETAIL_H
