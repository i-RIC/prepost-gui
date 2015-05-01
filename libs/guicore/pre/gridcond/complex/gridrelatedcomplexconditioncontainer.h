#ifndef GRIDRELATEDCOMPLEXCONDITIONCONTAINER_H
#define GRIDRELATEDCOMPLEXCONDITIONCONTAINER_H

#include "../base/gridrelatedconditioncontainert.h"
#include "../../../solverdef/solverdefinitiongridrelatedcomplexcondition.h"
#include "../../../project/projectcgnsfile.h"
#include "../../grid/grid.h"
#include <misc/stringtool.h>

#include <cgnslib.h>
#include <vtkDataArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>

class GridRelatedComplexConditionContainer : public GridRelatedConditionContainerT<int>
{
public:
	/// Constructor
	GridRelatedComplexConditionContainer(Grid* grid, SolverDefinitionGridRelatedComplexCondition* cond)
		: GridRelatedConditionContainerT<int>(grid, cond)
	{}
	/// Destructor
	virtual ~GridRelatedComplexConditionContainer() {}
	/// Returns the value at the specified index.
	int value(unsigned int index) {
		return dataArray()->GetValue(index);
	}
	/// Update the value at the specified index.
	void setValue(unsigned int index, int value) {
		dataArray()->SetValue(index, value);
	}
	void allocate() {
		vtkIntArray* da = dataArray();
		if (da == nullptr) {
			// not found maybe reset?
			da = vtkIntArray::New();
			da->SetName(iRIC::toStr(name()).c_str());
			if (condition()->position() == SolverDefinitionGridRelatedCondition::Node) {
				m_grid->vtkGrid()->GetPointData()->AddArray(da);
			} else {
				m_grid->vtkGrid()->GetCellData()->AddArray(da);
			}
			da->Delete();
		}
		da->Allocate(dataCount());
		int count = dataCount();
		for (int i = 0; i < count; ++i) {
			da->InsertValue(i, 0);
		}
	}

	vtkIntArray* dataArray() {
		vtkDataArray* da;
		if (condition()->position() == SolverDefinitionGridRelatedCondition::Node) {
			da = GridRelatedConditionContainer::m_grid->vtkGrid()->GetPointData()
					 ->GetArray(iRIC::toStr(GridRelatedComplexConditionContainer::name()).c_str());
		} else {
			da = GridRelatedConditionContainer::m_grid->vtkGrid()->GetCellData()
					 ->GetArray(iRIC::toStr(GridRelatedComplexConditionContainer::name()).c_str());
		}
		return vtkIntArray::SafeDownCast(da);
	}
	vtkIntArray* dataArrayCopy() {
		vtkIntArray* copy = vtkIntArray::New();
		copy->DeepCopy(dataArray());
		return copy;
	}
	unsigned int dataCount() const {
		if (condition()->position() == SolverDefinitionGridRelatedCondition::Node) {
			return m_grid->nodeCount();
		} else {
			return m_grid->cellCount();
		}
	}
	void setModified() {
		dataArray()->Modified();
	}
	bool getValueRange(double* min, double* max) {
		double range[2];
		dataArray()->GetRange(range);
		*min = range[0];
		*max = range[1];
		return true;
	}

protected:
	DataType_t dataType() {return Integer;}

private:
	SolverDefinitionGridRelatedComplexCondition* condition() const {
		return dynamic_cast<SolverDefinitionGridRelatedComplexCondition*>(m_condition);
	}
};

#endif // GRIDRELATEDCOMPLEXCONDITIONCONTAINER_H
