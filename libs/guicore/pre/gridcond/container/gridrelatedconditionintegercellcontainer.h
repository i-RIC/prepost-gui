#ifndef GRIDRELATEDCONDITIONINTEGERCELLCONTAINER_H
#define GRIDRELATEDCONDITIONINTEGERCELLCONTAINER_H

#include "../base/gridrelatedconditioncellcontainert.h"
#include <vtkIntArray.h>

class GridRelatedConditionIntegerCellContainer : public GridRelatedConditionCellContainerT<int, vtkIntArray>
{
public:
	GridRelatedConditionIntegerCellContainer(Grid* grid, SolverDefinitionGridRelatedConditionT<int>* cond)
		: GridRelatedConditionCellContainerT<int, vtkIntArray>(grid, cond)
	{}
protected:
	DataType_t dataType(){return Integer;}
};

#endif // GRIDRELATEDCONDITIONINTEGERCELLCONTAINER_H
