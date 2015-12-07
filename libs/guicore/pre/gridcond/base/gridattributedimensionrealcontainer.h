#ifndef GRIDATTRIBUTEDIMENSIONREALCONTAINER_H
#define GRIDATTRIBUTEDIMENSIONREALCONTAINER_H

#include "../../../guicore_global.h"
#include "gridattributedimensioncontainert.h"

class GUICOREDLL_EXPORT GridAttributeDimensionRealContainer : public GridAttributeDimensionContainerT<double>
{
public:
	GridAttributeDimensionRealContainer(SolverDefinitionGridAttributeDimension* def);
	~GridAttributeDimensionRealContainer();

private:
	DataType_t dataType() const override;
};

#endif // GRIDATTRIBUTEDIMENSIONREALCONTAINER_H
