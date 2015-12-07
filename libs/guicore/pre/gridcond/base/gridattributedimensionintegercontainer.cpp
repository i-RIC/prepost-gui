#include "gridattributedimensionintegercontainer.h"

GridAttributeDimensionIntegerContainer::GridAttributeDimensionIntegerContainer(SolverDefinitionGridAttributeDimension* def) :
	GridAttributeDimensionContainerT<int> (def)
{}

GridAttributeDimensionIntegerContainer::~GridAttributeDimensionIntegerContainer()
{}

DataType_t GridAttributeDimensionIntegerContainer::dataType() const
{
	return Integer;
}
