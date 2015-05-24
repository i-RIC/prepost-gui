#ifndef GRIDRELATEDCONDITIONINTEGERNODECONTAINER_H
#define GRIDRELATEDCONDITIONINTEGERNODECONTAINER_H

#include "../base/gridrelatedconditionnodecontainert.h"
#include <vtkIntArray.h>

class GridRelatedConditionIntegerNodeContainer : public GridRelatedConditionNodeContainerT<int, vtkIntArray>
{
public:
	GridRelatedConditionIntegerNodeContainer(Grid* grid, SolverDefinitionGridRelatedConditionT<int>* cond)
		: GridRelatedConditionNodeContainerT<int, vtkIntArray>(grid, cond)
	{}
protected:
	DataType_t dataType() const override {return Integer;}
};

#endif // GRIDRELATEDCONDITIONINTEGERNODECONTAINER_H
