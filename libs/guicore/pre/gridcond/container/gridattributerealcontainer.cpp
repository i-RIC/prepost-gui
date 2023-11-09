#include "gridattributerealcontainer.h"

GridAttributeRealContainer::GridAttributeRealContainer(v4InputGrid* grid, SolverDefinitionGridAttributeT<double>* cond) :
	GridAttributeContainerT<double, vtkDoubleArray> {grid, cond}
{}
