#include "gridattributestringcontainer.h"

GridAttributeStringContainer::GridAttributeStringContainer(v4InputGrid* grid, GridAttributeContainerIoI* io, SolverDefinitionGridAttributeT<std::string>* cond) :
	GridAttributeContainerT<std::string, vtkStringArray>(grid, io, cond)
{}
