#ifndef GRIDRELATEDCOMPLEXCONDITIONCONTAINER_H
#define GRIDRELATEDCOMPLEXCONDITIONCONTAINER_H

#include "../base/gridattributecontainert.h"

#include <vtkIntArray.h>

class SolverDefinitionGridComplexAttribute;

class GridComplexAttributeContainer : public GridAttributeContainerT<int>
{

public:
	/// Constructor
	GridComplexAttributeContainer(Grid* grid, SolverDefinitionGridComplexAttribute* cond);
	/// Destructor
	virtual ~GridComplexAttributeContainer();
	/// Returns the value at the specified index.
	int value(unsigned int index) override;
	/// Update the value at the specified index.
	void setValue(unsigned int index, int value) override;
	void allocate() override;

	vtkIntArray* dataArray() const override;
	vtkIntArray* dataArrayCopy() const override;
	unsigned int dataCount() const override;
	void setModified() override;
	bool getValueRange(double* min, double* max) override;

protected:
	DataType_t dataType() const override;

private:
	SolverDefinitionGridComplexAttribute* condition() const;
};

#endif // GRIDRELATEDCOMPLEXCONDITIONCONTAINER_H
