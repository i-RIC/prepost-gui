#ifndef GRIDRELATEDCONDITIONREALCELLCONTAINER_H
#define GRIDRELATEDCONDITIONREALCELLCONTAINER_H

#include "../base/gridrelatedconditioncellcontainert.h"
#include <vtkDoubleArray.h>

class GridRelatedConditionRealCellContainer : public GridRelatedConditionCellContainerT<double, vtkDoubleArray>
{
public:
	GridRelatedConditionRealCellContainer(Grid* grid, SolverDefinitionGridRelatedConditionT<double>* cond)
		: GridRelatedConditionCellContainerT<double, vtkDoubleArray>(grid, cond)
	{}
protected:
	DataType_t dataType() {return RealDouble;}
};

#endif // GRIDRELATEDCONDITIONREALCELLCONTAINER_H
