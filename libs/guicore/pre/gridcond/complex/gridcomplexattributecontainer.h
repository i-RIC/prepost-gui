#ifndef GRIDRELATEDCOMPLEXCONDITIONCONTAINER_H
#define GRIDRELATEDCOMPLEXCONDITIONCONTAINER_H

#include "../base/gridattributecontainert.h"

#include <vtkIntArray.h>

class SolverDefinitionGridComplexAttribute;

class GridComplexAttributeContainer : public GridAttributeContainerT<int>
{
public:
	GridComplexAttributeContainer(Grid* grid, SolverDefinitionGridComplexAttribute* cond);
	virtual ~GridComplexAttributeContainer();

	unsigned int dataCount() const override;

	int value(unsigned int index) const override;
	void setValue(unsigned int index, int value) override;

	void allocate() override;

	vtkIntArray* dataArray() const override;
	vtkIntArray* dataArrayCopy() const override;
	void setModified() override;
	bool getValueRange(double* min, double* max) override;

protected:
	DataType_t dataType() const override;

private:
	SolverDefinitionGridComplexAttribute* gridAttribute() const;
};

#endif // GRIDRELATEDCOMPLEXCONDITIONCONTAINER_H
