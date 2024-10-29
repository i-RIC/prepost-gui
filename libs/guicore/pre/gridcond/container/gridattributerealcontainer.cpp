#include "gridattributerealcontainer.h"

GridAttributeRealContainer::GridAttributeRealContainer(v4InputGrid* grid, GridAttributeContainerIoI* io, SolverDefinitionGridAttributeT<double>* cond) :
	GridAttributeContainerT<double, vtkDoubleArray> {grid, io, cond}
{}
