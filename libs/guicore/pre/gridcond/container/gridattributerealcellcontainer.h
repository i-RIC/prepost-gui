#ifndef GRIDATTRIBUTEREALCELLCONTAINER_H
#define GRIDATTRIBUTEREALCELLCONTAINER_H

#include "../base/gridattributecellcontainert.h"
#include <vtkDoubleArray.h>

class GridAttributeRealCellContainer : public GridAttributeCellContainerT<double, vtkDoubleArray>
{

public:
	GridAttributeRealCellContainer(Grid* grid, SolverDefinitionGridAttributeT<double>* cond)
		: GridAttributeCellContainerT<double, vtkDoubleArray>(grid, cond)
	{}
};

#endif // GRIDATTRIBUTEREALCELLCONTAINER_H
