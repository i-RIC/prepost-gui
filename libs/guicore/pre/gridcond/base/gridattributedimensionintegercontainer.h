#ifndef GRIDATTRIBUTEDIMENSIONINTEGERCONTAINER_H
#define GRIDATTRIBUTEDIMENSIONINTEGERCONTAINER_H

#include "../../../guicore_global.h"
#include "gridattributedimensioncontainert.h"

class GUICOREDLL_EXPORT GridAttributeDimensionIntegerContainer : public GridAttributeDimensionContainerT<int>
{
public:
	GridAttributeDimensionIntegerContainer(SolverDefinitionGridAttributeDimension* def);
	~GridAttributeDimensionIntegerContainer();

private:
	DataType_t dataType() const override;
};

#endif // GRIDATTRIBUTEDIMENSIONINTEGERCONTAINER_H
