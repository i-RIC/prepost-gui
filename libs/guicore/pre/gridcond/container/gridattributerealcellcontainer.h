#ifndef GRIDRELATEDCONDITIONREALCELLCONTAINER_H
#define GRIDRELATEDCONDITIONREALCELLCONTAINER_H

#include "../base/gridattributecellcontainert.h"
#include <vtkDoubleArray.h>

class GridAttributeRealCellContainer : public GridAttributeCellContainerT<double, vtkDoubleArray>
{

public:
	GridAttributeRealCellContainer(Grid* grid, SolverDefinitionGridAttributeT<double>* cond)
		: GridAttributeCellContainerT<double, vtkDoubleArray>(grid, cond)
	{}

protected:
	DataType_t dataType() const override {return RealDouble;}
};

#endif // GRIDRELATEDCONDITIONREALCELLCONTAINER_H
