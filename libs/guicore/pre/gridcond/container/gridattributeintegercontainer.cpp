#include "gridattributeintegercontainer.h"

GridAttributeIntegerContainer::GridAttributeIntegerContainer(v4InputGrid* grid, GridAttributeContainerIoI* io, SolverDefinitionGridAttributeT<int>* cond) :
	GridAttributeContainerT<int, vtkIntArray> {grid, io, cond}
{}
