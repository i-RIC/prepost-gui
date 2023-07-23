#ifndef GRIDATTRIBUTEREALIFACECONTAINER_H
#define GRIDATTRIBUTEREALIFACECONTAINER_H

#include "../base/gridattributenodecontainert.h"
#include <vtkDoubleArray.h>

class GridAttributeRealIFaceContainer : public GridAttributeNodeContainerT<double, vtkDoubleArray>
{
public:
	GridAttributeRealIFaceContainer(Grid* grid, SolverDefinitionGridAttributeT<double>* cond)
		: GridAttributeNodeContainerT<double, vtkDoubleArray>(grid, cond)
	{}
};

#endif // GRIDATTRIBUTEREALIFACECONTAINER_H
