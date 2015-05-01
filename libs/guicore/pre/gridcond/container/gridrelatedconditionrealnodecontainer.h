#ifndef GRIDRELATEDCONDITIONREALNODECONTAINER_H
#define GRIDRELATEDCONDITIONREALNODECONTAINER_H

#include "../base/gridrelatedconditionnodecontainert.h"
#include <vtkDoubleArray.h>

class GridRelatedConditionRealNodeContainer : public GridRelatedConditionNodeContainerT<double, vtkDoubleArray>
{
public:
	GridRelatedConditionRealNodeContainer(Grid* grid, SolverDefinitionGridRelatedConditionT<double>* cond)
		: GridRelatedConditionNodeContainerT<double, vtkDoubleArray>(grid, cond)
	{}
protected:
	DataType_t dataType() {return RealDouble;}
};

#endif // GRIDRELATEDCONDITIONREALNODECONTAINER_H
