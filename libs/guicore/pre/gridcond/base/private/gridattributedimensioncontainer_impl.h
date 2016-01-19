#ifndef GRIDATTRIBUTEDIMENSIONCONTAINER_IMPL_H
#define GRIDATTRIBUTEDIMENSIONCONTAINER_IMPL_H

#include "../gridattributedimensioncontainer.h"

class GridAttributeDimensionContainer::Impl
{
public:
	Impl(SolverDefinitionGridAttributeDimension* def);

	SolverDefinitionGridAttributeDimension* m_definition;
	int m_currentIndex;
};

#endif // GRIDATTRIBUTEDIMENSIONCONTAINER_IMPL_H

