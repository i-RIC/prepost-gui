#include "gridattributeintegercontainer.h"

GridAttributeIntegerContainer::GridAttributeIntegerContainer(v4InputGrid* grid, SolverDefinitionGridAttributeT<int>* cond) :
	GridAttributeContainerT<int, vtkIntArray> {grid, cond}
{}
