#ifndef GRIDRELATEDCONDITIONCELLCONTAINERT_H
#define GRIDRELATEDCONDITIONCELLCONTAINERT_H

#include "gridattributecontainert.h"
#include "../../grid/grid.h"
#include <misc/stringtool.h>
#include <vtkCellData.h>

template <class V, class DA>
class GridAttributeCellContainerT : public GridAttributeContainerT<V>
{

public:
	/// Constructor
	GridAttributeCellContainerT(Grid* grid, SolverDefinitionGridAttributeT<V>* cond) :
		GridAttributeContainerT<V> {grid, cond}
	{
		DA* da = DA::New();
		da->SetName(iRIC::toStr(GridAttributeContainerT<V>::name()).c_str());
		grid->vtkGrid()->GetCellData()->AddArray(da);
		da->Delete();
	}
	/// Destructor
	virtual ~GridAttributeCellContainerT() {}
	void allocate() override {
		vtkDataArray* a = GridAttributeContainerT<V>::m_grid->vtkGrid()->GetCellData()->GetArray(iRIC::toStr(GridAttributeContainerT<V>::name()).c_str());
		DA* da = DA::SafeDownCast(a);
		if (da == nullptr) {
			// not found maybe reset?
			da = DA::New();
			da->SetName(iRIC::toStr(GridAttributeContainerT<V>::name()).c_str());
			GridAttributeContainerT<V>::m_grid->vtkGrid()->GetCellData()->AddArray(da);
			da->Delete();
		}
		da->Allocate(dataCount());
		int count = dataCount();
		for (int i = 0; i < count; ++i) {
			da->InsertValue(i, 0);
		}
	}
	/**
	 * @param index The index of grid cell.
	 */
	V value(unsigned int index) override {
		return dataArray()->GetValue(static_cast<vtkIdType>(index));
	}
	/// The setter function of values.
	void setValue(unsigned int index, V val) override {
		dataArray()->SetValue(static_cast<vtkIdType>(index), val);
	}
	DA* dataArray() const override {
		vtkPointSet* ps = const_cast<vtkPointSet*> (GridAttributeContainerT<V>::m_grid->vtkGrid());
		vtkDataArray* da = ps->GetCellData()->GetArray(iRIC::toStr(GridAttributeContainerT<V>::name()).c_str());
		DA* da2 = DA::SafeDownCast(da);
		return da2;
	}
	DA* dataArrayCopy() const override {
		DA* copy = DA::New();
		DA* orig = dataArray();
		copy->DeepCopy(orig);
		copy->SetName(orig->GetName());
		return copy;
	}
	unsigned int dataCount() const override {
		return GridAttributeContainerT<V>::m_grid->cellCount();
	}
};

#endif // GRIDRELATEDCONDITIONCELLCONTAINERT_H
