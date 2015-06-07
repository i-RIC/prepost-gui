#ifndef GRIDRELATEDCOMPLEXCONDITIONCONTAINER_H
#define GRIDRELATEDCOMPLEXCONDITIONCONTAINER_H

#include "../base/gridattributecontainert.h"
#include "../../../solverdef/solverdefinitiongridcomplexattribute.h"
#include "../../../project/projectcgnsfile.h"
#include "../../grid/grid.h"
#include <misc/stringtool.h>

#include <cgnslib.h>
#include <vtkDataArray.h>
#include <vtkIntArray.h>
#include <vtkPointData.h>
#include <vtkCellData.h>

class GridComplexAttributeContainer : public GridAttributeContainerT<int>
{
public:
	/// Constructor
	GridComplexAttributeContainer(Grid* grid, SolverDefinitionGridComplexAttribute* cond)
		: GridAttributeContainerT<int>(grid, cond)
	{}
	/// Destructor
	virtual ~GridComplexAttributeContainer() {}
	/// Returns the value at the specified index.
	int value(unsigned int index) override {
		return dataArray()->GetValue(index);
	}
	/// Update the value at the specified index.
	void setValue(unsigned int index, int value) override {
		dataArray()->SetValue(index, value);
	}
	void allocate() override {
		vtkIntArray* da = dataArray();
		if (da == nullptr) {
			// not found maybe reset?
			da = vtkIntArray::New();
			da->SetName(iRIC::toStr(name()).c_str());
			if (condition()->position() == SolverDefinitionGridAttribute::Node) {
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

	vtkIntArray* dataArray() const override {
		vtkPointSet* ps = const_cast<vtkPointSet*> (GridAttributeContainer::m_grid->vtkGrid());
		vtkDataArray* da = nullptr;
		if (condition()->position() == SolverDefinitionGridAttribute::Node) {
			da = ps->GetPointData()->GetArray(iRIC::toStr(GridComplexAttributeContainer::name()).c_str());
		} else {
			da = ps->GetCellData()->GetArray(iRIC::toStr(GridComplexAttributeContainer::name()).c_str());
		}
		return vtkIntArray::SafeDownCast(da);
	}
	vtkIntArray* dataArrayCopy() const override {
		vtkIntArray* copy = vtkIntArray::New();
		copy->DeepCopy(dataArray());
		return copy;
	}
	unsigned int dataCount() const override {
		if (condition()->position() == SolverDefinitionGridAttribute::Node) {
			return m_grid->nodeCount();
		} else {
			return m_grid->cellCount();
		}
	}
	void setModified() override {
		dataArray()->Modified();
	}
	bool getValueRange(double* min, double* max) override {
		double range[2];
		dataArray()->GetRange(range);
		*min = range[0];
		*max = range[1];
		return true;
	}

protected:
	DataType_t dataType() const override {return Integer;}

private:
	SolverDefinitionGridComplexAttribute* condition() const {
		return dynamic_cast<SolverDefinitionGridComplexAttribute*>(m_condition);
	}
};

#endif // GRIDRELATEDCOMPLEXCONDITIONCONTAINER_H
