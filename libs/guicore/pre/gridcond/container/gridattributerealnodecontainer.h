#ifndef GRIDATTRIBUTEREALNODECONTAINER_H
#define GRIDATTRIBUTEREALNODECONTAINER_H

#include "../base/gridattributenodecontainert.h"
#include <vtkDoubleArray.h>

class GridAttributeRealNodeContainer : public GridAttributeNodeContainerT<double, vtkDoubleArray>
{

public:
	GridAttributeRealNodeContainer(Grid* grid, SolverDefinitionGridAttributeT<double>* cond)
		: GridAttributeNodeContainerT<double, vtkDoubleArray>(grid, cond)
	{}

protected:
	DataType_t dataType() const override {return RealDouble;}
};

#endif // GRIDATTRIBUTEREALNODECONTAINER_H
