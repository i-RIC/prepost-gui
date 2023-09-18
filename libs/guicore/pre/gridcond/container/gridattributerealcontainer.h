#ifndef GRIDATTRIBUTEREALCONTAINER_H
#define GRIDATTRIBUTEREALCONTAINER_H

#include "../base/gridattributecontainert.h"

#include <vtkDoubleArray.h>

class GridAttributeRealContainer : public GridAttributeContainerT<double, vtkDoubleArray>
{
public:
	GridAttributeRealContainer(v4InputGrid* grid, SolverDefinitionGridAttributeT<double>* cond);
};

#endif // GRIDATTRIBUTEREALCONTAINER_H
