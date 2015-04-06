#ifndef GRIDRELATEDCONDITIONNODECONTAINERT_H
#define GRIDRELATEDCONDITIONNODECONTAINERT_H

#include "gridrelatedconditioncontainert.h"
#include "../../../solverdef/solverdefinitiongridrelatedconditiont.h"
#include "../../grid/grid.h"
#include <misc/stringtool.h>
#include <vtkPointData.h>

template <class V, class DA>
class GridRelatedConditionNodeContainerT : public GridRelatedConditionContainerT<V>
{
public:
	/// Constructor
	GridRelatedConditionNodeContainerT(Grid* grid, SolverDefinitionGridRelatedConditionT<V>* cond)
		: GridRelatedConditionContainerT<V>(grid, cond)
	{
		DA* da = DA::New();
		da->SetName(iRIC::toStr(GridRelatedConditionContainerT<V>::name()).c_str());
		grid->vtkGrid()->GetPointData()->AddArray(da);
		da->Delete();
	}
	/// Destructor
	virtual ~GridRelatedConditionNodeContainerT(){}
	void allocate(){
		vtkDataArray* a = GridRelatedConditionContainerT<V>::m_grid->vtkGrid()->GetPointData()->GetArray(iRIC::toStr(GridRelatedConditionContainerT<V>::name()).c_str());
		DA* da = DA::SafeDownCast(a);
		if (da == 0){
			// not found maybe reset?
			da = DA::New();
			da->SetName(iRIC::toStr(GridRelatedConditionContainerT<V>::name()).c_str());
			m_grid->vtkGrid()->GetPointData()->AddArray(da);
			da->Delete();
		}
		da->Allocate(dataCount());
		int count = dataCount();
		for (int i = 0; i < count; ++i){
			da->InsertValue(i, 0);
		}
	}
	/// Value
	/**
	 * @param index The index of grid cell.
	 */
	V value(unsigned int index){
		return dataArray()->GetValue(static_cast<vtkIdType>(index));
	}
	/// The setter function of values.
	void setValue(unsigned int index, V val){
		dataArray()->SetValue(static_cast<vtkIdType>(index), val);
	}
	DA* dataArray(){
		vtkDataArray* da = GridRelatedConditionContainerT<V>::m_grid->vtkGrid()->GetPointData()->GetArray(iRIC::toStr(GridRelatedConditionContainerT<V>::name()).c_str());
		DA* da2 = DA::SafeDownCast(da);
		return da2;
	}
	DA* dataArrayCopy(){
		DA* copy = DA::New();
		copy->DeepCopy(dataArray());
		return copy;
	}
	const unsigned int dataCount() const {
		return GridRelatedConditionContainerT<V>::m_grid->nodeCount();
	}
};

#endif // GRIDRELATEDCONDITIONNODECONTAINERT_H
