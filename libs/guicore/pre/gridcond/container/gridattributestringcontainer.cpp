#include "gridattributestringcontainer.h"

GridAttributeStringContainer::GridAttributeStringContainer(v4InputGrid* grid, SolverDefinitionGridAttributeT<std::string>* cond) :
	GridAttributeContainerT<std::string, vtkStringArray>(grid, cond)
{}
