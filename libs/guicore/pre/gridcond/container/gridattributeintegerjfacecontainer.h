#ifndef GRIDATTRIBUTEINTEGERJFACECONTAINER_H
#define GRIDATTRIBUTEINTEGERJFACECONTAINER_H

#include "../base/gridattributecellcontainert.h"
#include <vtkIntArray.h>

class GridAttributeIntegerJFaceContainer : public GridAttributeCellContainerT<int, vtkIntArray>
{
public:
	GridAttributeIntegerJFaceContainer(Grid* grid, SolverDefinitionGridAttributeT<int>* cond)
		: GridAttributeCellContainerT<int, vtkIntArray>(grid, cond)
	{}
};

#endif // GRIDATTRIBUTEINTEGERJFACECONTAINER_H
