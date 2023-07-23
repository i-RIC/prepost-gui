#ifndef GRIDATTRIBUTEINTEGERIFACECONTAINER_H
#define GRIDATTRIBUTEINTEGERIFACECONTAINER_H

#include "../base/gridattributecellcontainert.h"
#include <vtkIntArray.h>

class GridAttributeIntegerIFaceContainer : public GridAttributeCellContainerT<int, vtkIntArray>
{
public:
	GridAttributeIntegerIFaceContainer(Grid* grid, SolverDefinitionGridAttributeT<int>* cond)
		: GridAttributeCellContainerT<int, vtkIntArray>(grid, cond)
	{}
};

#endif // GRIDATTRIBUTEINTEGERIFACECONTAINER_H
