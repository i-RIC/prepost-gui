#ifndef GRIDATTRIBUTEINTEGERNODECONTAINER_H
#define GRIDATTRIBUTEINTEGERNODECONTAINER_H

#include "../base/gridattributenodecontainert.h"
#include <vtkIntArray.h>

class GridAttributeIntegerNodeContainer : public GridAttributeNodeContainerT<int, vtkIntArray>
{

public:
	GridAttributeIntegerNodeContainer(Grid* grid, SolverDefinitionGridAttributeT<int>* cond)
		: GridAttributeNodeContainerT<int, vtkIntArray>(grid, cond)
	{}
};

#endif // GRIDATTRIBUTEINTEGERNODECONTAINER_H
