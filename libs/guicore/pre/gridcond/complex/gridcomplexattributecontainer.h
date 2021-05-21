#ifndef GRIDRELATEDCOMPLEXCONDITIONCONTAINER_H
#define GRIDRELATEDCOMPLEXCONDITIONCONTAINER_H

#include "../../../guicore_global.h"
#include "../base/gridattributecontainert.h"

#include <vtkIntArray.h>

class GridComplexConditionGroup;
class SolverDefinitionGridComplexAttribute;

class GUICOREDLL_EXPORT GridComplexAttributeContainer : public GridAttributeContainerT<int>
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

	std::vector<GridComplexConditionGroup*> groups() const;

private:
	SolverDefinitionGridComplexAttribute* gridAttribute() const;
};

#endif // GRIDRELATEDCOMPLEXCONDITIONCONTAINER_H
