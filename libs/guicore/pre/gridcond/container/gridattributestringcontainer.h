#ifndef GRIDATTRIBUTESTRINGCONTAINER_H
#define GRIDATTRIBUTESTRINGCONTAINER_H

#include "../base/gridattributecontainert.h"

#include <vtkStringArray.h>

#include <string>

class GridAttributeStringContainer : public GridAttributeContainerT<std::string, vtkStringArray>
{
public:
	GridAttributeStringContainer(v4InputGrid* grid, GridAttributeContainerIoI* io, SolverDefinitionGridAttributeT<std::string>* cond);
};

#endif // GRIDATTRIBUTESTRINGCONTAINER_H
