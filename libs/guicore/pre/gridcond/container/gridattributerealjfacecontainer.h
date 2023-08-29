#ifndef GRIDATTRIBUTEREALJFACECONTAINER_H
#define GRIDATTRIBUTEREALJFACECONTAINER_H

#include "../base/gridattributenodecontainert.h"
#include <vtkDoubleArray.h>

class GridAttributeRealJFaceContainer : public GridAttributeNodeContainerT<double, vtkDoubleArray>
{
public:
	GridAttributeRealJFaceContainer(Grid* grid, SolverDefinitionGridAttributeT<double>* cond)
		: GridAttributeNodeContainerT<double, vtkDoubleArray>(grid, cond)
	{}
};

#endif // GRIDATTRIBUTEREALJFACECONTAINER_H
