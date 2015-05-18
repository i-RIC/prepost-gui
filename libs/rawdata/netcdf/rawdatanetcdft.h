#ifndef RAWDATANETCDFT_H
#define RAWDATANETCDFT_H

#include "rawdatanetcdf.h"
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>
#include <guicore/pre/gridcond/base/gridrelatedconditiondimensionscontainer.h>
#include <guicore/pre/base/preprocessorrawdatadataiteminterface.h>
#include <misc/stringtool.h>
#include <vtkSmartPointer.h>
#include <vtkCellData.h>
#include <vtkVertex.h>
#include <vtkIntArray.h>
#include <vtkDoubleArray.h>

template <class V, class DA>
class RawDataNetcdfT : public RawDataNetcdf
{
public:
	RawDataNetcdfT(ProjectDataItem* d, RawDataCreator* creator, SolverDefinitionGridRelatedCondition* condition)
		: RawDataNetcdf(d, creator, condition) {
		vtkSmartPointer<DA> values = vtkSmartPointer<DA>::New();
		values->SetName("values");
		m_grid->GetCellData()->AddArray(values);
		m_grid->GetCellData()->SetActiveScalars("values");
	}
	DA* vtkValues() {
		vtkDataArray* da = m_grid->GetCellData()->GetArray("values");
		return DA::SafeDownCast(da);
	}
	V value(vtkIdType index) {return vtkValues()->GetValue(index);}
	void setValue(vtkIdType index, V val) {return vtkValues()->SetValue(index, val);}
	bool getValueRange(double* min, double* max) {
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
	int outputValues(int ncid, int varid, V* vals) {
		return nc_put_var(ncid, varid, vals);
	}
	int outputValues(int ncid, int varid, const QList<int>& indices, V* vals) {
		size_t* start = new size_t[indices.size() + 2];
		size_t* len = new size_t[indices.size() + 2];

		for (int i = 0; i < indices.size(); ++i) {
			*(start + indices.size() - 1 - i) = indices.at(i);
			*(len + indices.size() - 1 - i) = 1;
		}
		*(start + indices.size()) = 0;
		*(start + indices.size() + 1) = 0;
		if (m_coordinateSystemType == XY) {
			*(len + indices.size()) = m_yValues.size();
			*(len + indices.size() + 1) = m_xValues.size();
		} else if (m_coordinateSystemType == LonLat) {
			*(len + indices.size()) = m_lonValues.size();
			*(len + indices.size() + 1) = m_latValues.size();
		}
		int ret = nc_put_vara(ncid, varid, start, len, vals);

		delete start;
		delete len;
		return ret;
	}
	virtual V missingValue() = 0;

protected:
	void doHandleDimensionCurrentIndexChange(int /*oldIndex*/, int newIndex) {
		std::string fname = iRIC::toStr(filename());
		int ncid, ret, varId;

		ret = nc_open(fname.c_str(), NC_NOWRITE, &ncid);
		ret = getValueVarId(ncid, &varId);

		GridRelatedConditionDimensionsContainer* dims = dimensions();

		// @todo currently, netcdf does not support edit, so no save done.

		QList<int> indices = dims->calculateIndices(newIndex);
		int ndims = dims->containers().size() + 2;
		size_t* start = new size_t[ndims];
		size_t* len = new size_t[ndims];
		size_t bufferSize = 0;
		V* vals;

		for (int i = 0; i < indices.size(); ++i) {
			*(start + indices.size() - 1 - i) = indices.at(i);
			*(len + indices.size() - 1 - i) = 1;
		}
		*(start + indices.size()) = 0;
		*(start + indices.size() + 1) = 0;
		if (m_coordinateSystemType == XY) {
			*(len + indices.size()) = m_yValues.size();
			*(len + indices.size() + 1) = m_xValues.size();
			bufferSize = m_xValues.size() * m_yValues.size();
		} else if (m_coordinateSystemType == LonLat) {
			*(len + indices.size()) = m_latValues.size();
			*(len + indices.size() + 1) = m_lonValues.size();
			bufferSize = m_lonValues.size() * m_latValues.size();
		}
		vals = new V[bufferSize];

		ret = nc_get_vara(ncid, varId, start, len, vals);
		// put the value got into the dataarray.
		DA* vtkVals = vtkValues();
		vtkVals->Reset();
		vtkVals->Allocate(bufferSize);
		for (size_t i = 0; i < bufferSize; ++i) {
			V val = *(vals + i);
			vtkVals->InsertNextValue(val);
		}
		vtkVals->Modified();
		viewOperationEndedGlobal(graphicsView());

		delete vals;
		delete len;
		delete start;

		nc_close(ncid);
		dynamic_cast<PreProcessorRawdataDataItemInterface*>(parent())->informValueRangeChange();

		Q_UNUSED(ret)
	}

	void doHandleDimensionValuesChange(GridRelatedConditionDimensionContainer* /*cont*/, const QList<QVariant>& /*before*/, const QList<QVariant>& /*after*/) {

	}
};

class RawDataNetcdfInteger : public RawDataNetcdfT<int, vtkIntArray>
{
public:
	RawDataNetcdfInteger(ProjectDataItem* d, RawDataCreator* creator, SolverDefinitionGridRelatedCondition* condition)
		: RawDataNetcdfT(d, creator, condition) {
	}
	int missingValue() {
		return NC_FILL_INT;
	}
	double thresholdValue() {
		int mval = missingValue();
		if (mval > 0) {return mval - 1;}
		return mval + 1;
	}
};

class RawDataNetcdfReal : public RawDataNetcdfT<double, vtkDoubleArray>
{
public:
	RawDataNetcdfReal(ProjectDataItem* d, RawDataCreator* creator, SolverDefinitionGridRelatedCondition* condition)
		: RawDataNetcdfT(d, creator, condition) {
	}
	double missingValue() {
		return NC_FILL_DOUBLE;
	}
	double thresholdValue() {
		double mval = missingValue();
		return mval * 0.999;
	}
};

#endif // RAWDATANETCDFT_H
