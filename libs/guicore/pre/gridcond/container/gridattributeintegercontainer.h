#ifndef GRIDATTRIBUTEINTEGERCONTAINER_H
#define GRIDATTRIBUTEINTEGERCONTAINER_H

#include "../base/gridattributecontainert.h"

#include <vtkIntArray.h>

class GridAttributeIntegerContainer : public GridAttributeContainerT<int, vtkIntArray>
{
public:
	GridAttributeIntegerContainer(v4InputGrid* grid, GridAttributeContainerIoI* io, SolverDefinitionGridAttributeT<int>* cond);
};

#endif // GRIDATTRIBUTEINTEGERCONTAINER_H
