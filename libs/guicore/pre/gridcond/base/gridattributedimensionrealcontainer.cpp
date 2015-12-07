#include "gridattributedimensionrealcontainer.h"

GridAttributeDimensionRealContainer::GridAttributeDimensionRealContainer(SolverDefinitionGridAttributeDimension* def) :
	GridAttributeDimensionContainerT<double> (def)
{}

GridAttributeDimensionRealContainer::~GridAttributeDimensionRealContainer()
{}

DataType_t GridAttributeDimensionRealContainer::dataType() const
{
	return RealDouble;
}
