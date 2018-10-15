#ifndef GEODATANETCDFT_DETAIL_H
#define GEODATANETCDFT_DETAIL_H

#include "../geodatanetcdft.h"

#include <guicore/pre/gridcond/base/gridattributedimensionscontainer.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <misc/stringtool.h>

#include <vtkCellData.h>

template <class V, class DA>
GeoDataNetcdfT<V, DA>::GeoDataNetcdfT(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition) :
	GeoDataNetcdf {d, creator, condition}
{
	vtkSmartPointer<DA> values = vtkSmartPointer<DA>::New();
	values->SetName("values");
	m_grid->GetCellData()->AddArray(values);
	m_grid->GetCellData()->SetActiveScalars("values");
}

template <class V, class DA>
GeoDataNetcdfT<V, DA>::~GeoDataNetcdfT()
{}

template <class V, class DA>
DA* GeoDataNetcdfT<V, DA>::vtkValues() const
{
	vtkDataArray* da = m_grid->GetCellData()->GetArray("values");
	return DA::SafeDownCast(da);
}

template <class V, class DA>
V GeoDataNetcdfT<V, DA>::value(vtkIdType index) const
{
	return vtkValues()->GetValue(index);
}

template <class V, class DA>
void GeoDataNetcdfT<V, DA>::setValue(vtkIdType index, V val)
{
	return vtkValues()->SetValue(index, val);
}

template <class V, class DA>
bool GeoDataNetcdfT<V, DA>::getValueRange(double* min, double* max)
{
	if (vtkValues()->GetNumberOfTuples() == 0) {return false;}

	DA* vtkVals = vtkValues();
	bool first = true;
	for (vtkIdType i = 0; i < vtkVals->GetNumberOfTuples(); ++i) {
		V val = vtkVals->GetValue(i);
		if (val == missingValue()) {continue;}
		if (val < *min || first) {
			*min = val;
		}
		if (val > *max || first) {
			*max = val;
		}
		first = false;
	}
	if (first) {return false;}

	return true;
}

template <class V, class DA>
int GeoDataNetcdfT<V, DA>::outputValues(int ncid, int varid, V* vals)
{
	return nc_put_var(ncid, varid, vals);
}

template <class V, class DA>
int GeoDataNetcdfT<V, DA>::outputValues(int ncid, int varid, const std::vector<int>& indices, V* vals)
{
	std::vector<size_t> start(indices.size() + 2);
	std::vector<size_t> len(indices.size() + 2);

	for (int i = 0; i < indices.size(); ++i) {
		start[indices.size() - 1 - i] = indices.at(i);
		len[indices.size() - 1 - i] = 1;
	}
	start[indices.size()] = 0;
	start[indices.size() + 1] = 0;
	if (m_coordinateSystemType == XY) {
		len[indices.size()] = m_yValues.size();
		len[indices.size() + 1] = m_xValues.size();
	} else if (m_coordinateSystemType == LonLat) {
		len[indices.size()] = m_lonValues.size();
		len[indices.size() + 1] = m_latValues.size();
	}
	int ret = nc_put_vara(ncid, varid, start.data(), len.data(), vals);
	return ret;
}

template <class V, class DA>
void GeoDataNetcdfT<V, DA>::doHandleDimensionCurrentIndexChange(int /*oldIndex*/, int newIndex)
{
	std::string fname = iRIC::toStr(filename());
	int ncid, ret, varId;

	ret = nc_open(fname.c_str(), NC_NOWRITE, &ncid);
	if (ret != NC_NOERR) {return;}

	ret = getValueVarId(ncid, &varId);

	GridAttributeDimensionsContainer* dims = dimensions();

	// @todo currently, netcdf does not support edit, so no save done.

	std::vector<int> indices = dims->calculateIndices(newIndex);
	int ndims = static_cast<int>(dims->containers().size()) + 2;
	std::vector<size_t> start(ndims);
	std::vector<size_t> len(ndims);
	size_t bufferSize = 0;
	std::vector<V> vals;

	for (int i = 0; i < indices.size(); ++i) {
		start[indices.size() - 1 - i] = indices.at(i);
		len[indices.size() - 1 - i] = 1;
	}
	start[indices.size()] = 0;
	start[indices.size() + 1] = 0;
	if (m_coordinateSystemType == XY) {
		len[indices.size()] = m_yValues.size();
		len[indices.size() + 1] = m_xValues.size();
		bufferSize = m_xValues.size() * m_yValues.size();
	} else if (m_coordinateSystemType == LonLat) {
		len[indices.size()] = m_latValues.size();
		len[indices.size() + 1] = m_lonValues.size();
		bufferSize = m_lonValues.size() * m_latValues.size();
	}
	vals.assign(bufferSize, 0);

	ret = nc_get_vara(ncid, varId, start.data(), len.data(), vals.data());
	// put the value got into the dataarray.
	DA* vtkVals = vtkValues();
	vtkVals->Reset();
	vtkVals->Allocate(bufferSize);
	for (size_t i = 0; i < bufferSize; ++i) {
		vtkVals->InsertNextValue(vals[i]);
	}
	vtkVals->Modified();
	viewOperationEndedGlobal(graphicsView());

	nc_close(ncid);
	dynamic_cast<PreProcessorGeoDataDataItemInterface*>(parent())->informValueRangeChange();
}

template <class V, class DA>
void GeoDataNetcdfT<V, DA>::doHandleDimensionValuesChange(GridAttributeDimensionContainer* /*cont*/, const std::vector<QVariant>& /*before*/, const std::vector<QVariant>& /*after*/)
{
	// @todo implement this
}

#endif // GEODATANETCDFT_DETAIL_H

