#ifndef GRIDATTRIBUTEINTEGERCELLCONTAINER_H
#define GRIDATTRIBUTEINTEGERCELLCONTAINER_H

#include "../base/gridattributecellcontainert.h"
#include <vtkIntArray.h>

class GridAttributeIntegerCellContainer : public GridAttributeCellContainerT<int, vtkIntArray>
{

public:
	GridAttributeIntegerCellContainer(Grid* grid, SolverDefinitionGridAttributeT<int>* cond)
		: GridAttributeCellContainerT<int, vtkIntArray>(grid, cond)
	{}

protected:
	DataType_t dataType() const override {return Integer;}
};

#endif // GRIDATTRIBUTEINTEGERCELLCONTAINER_H
