#ifndef GRIDRELATEDCONDITIONINTEGERNODECONTAINER_H
#define GRIDRELATEDCONDITIONINTEGERNODECONTAINER_H

#include "../base/gridattributenodecontainert.h"
#include <vtkIntArray.h>

class GridAttributeIntegerNodeContainer : public GridAttributeNodeContainerT<int, vtkIntArray>
{

public:
	GridAttributeIntegerNodeContainer(Grid* grid, SolverDefinitionGridAttributeT<int>* cond)
		: GridAttributeNodeContainerT<int, vtkIntArray>(grid, cond)
	{}

protected:
	DataType_t dataType() const override {return Integer;}
};

#endif // GRIDRELATEDCONDITIONINTEGERNODECONTAINER_H
